#include "template_manager_impl.h"

#include <archive.h>
#include <archive_entry.h>
#include <curl/curl.h>
#include <fmt/format.h>

#include <cstdlib>
#include <fstream>
#include <regex>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <streambuf>
#include <unistd.h>
#include <vector>

namespace {

size_t write_data(void* ptr, size_t size, size_t nmemb, void* stream) {
    size_t written = fwrite(ptr, size, nmemb, (FILE*)stream);
    return written;
}

int copy_data(struct archive* ar, struct archive* aw) {
    int r;
    const void* buff;
    size_t size;
#if ARCHIVE_VERSION_NUMBER >= 3000000
    int64_t offset;
#else
    off_t offset;
#endif

    for (;;) {
        r = archive_read_data_block(ar, &buff, &size, &offset);
        if (r == ARCHIVE_EOF) {
            return (ARCHIVE_OK);
        }
        if (r != ARCHIVE_OK) {
            return (r);
        }
        r = archive_write_data_block(aw, buff, size, offset);
        if (r != ARCHIVE_OK) {
            fmt::print(stderr, "archive_write_data_block(): {}\n",
                       archive_error_string(aw));
            return (r);
        }
    }
}

} // namespace

namespace cpgen {

TemplateManager::pImpl::pImpl() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    if (not std::filesystem::exists(templateRootPath())) {
        update();
    }
}

TemplateManager::pImpl::~pImpl() {
    curl_global_cleanup();
}

bool TemplateManager::pImpl::update() {
    bool all_ok{true};
    all_ok &= downloadTemplate();
    all_ok &= extractTemplate();
    return all_ok;
}

void TemplateManager::pImpl::createProject(const ProjectParameters& project) {
    namespace fs = std::filesystem;
    fs::path origin = templateRootPath() / fs::path{"project"};
    fs::path destination = fs::path{project.root_path} / fs::path{project.name};
    fs::copy(origin, destination,
             fs::copy_options::recursive | fs::copy_options::skip_existing);

    const auto conan_pkgs =
        fmt::format("\"{}\"", fmt::join(project.conan_pkgs, "\", \""));

    const auto cmake_pkgs = [&] {
        std::string s;
        for (const auto& pkg : project.cmake_pkgs) {
            if (pkg.empty()) {
                continue;
            }
            s += fmt::format("find_package({})\n", pkg);
        }
        return s;
    }();

    const auto dictionnary = std::map<std::string, std::string>{
        {"project_name", project.name},
        {"project_description", project.description},
        {"project_version", project.version},
        {"conan_pkgs", conan_pkgs},
        {"cmake_pkgs", cmake_pkgs}};

    searchAndReplace(destination, std::move(dictionnary));
}

void TemplateManager::pImpl::createLibrary(const LibraryParameters& library,
                                           std::filesystem::path project_root) {
    namespace fs = std::filesystem;

    const auto template_root = [this, &library]() {
        switch (library.type) {
        case LibraryType::HeaderOnly:
            return templateRootPath() / "library/header_only";
        case LibraryType::Module:
            return templateRootPath() / "library/module";
        case LibraryType::Static:
            [[fallthrough]];
        case LibraryType::Shared:
            return templateRootPath() / "library/static_shared";
            break;
        }
        return fs::path(); // fix missing return warning
    }();

    const auto library_type_str = [](LibraryType type) -> std::string {
        switch (type) {
        case LibraryType::HeaderOnly:
            return "INTERFACE";
        case LibraryType::Module:
            return "MODULE";
        case LibraryType::Static:
            return "STATIC";
        case LibraryType::Shared:
            return "SHARED";
        }
        return ""; // fix missing return warning
    };

    const auto dependencies_list =
        fmt::format("{}", fmt::join(library.dependencies, "\n\t\t"));

    const auto dictionnary = std::map<std::string, std::string>{
        {"component_name", library.name},
        {"component_std", library.standard},
        {"component_type", library_type_str(library.type)},
        {"component_dependencies", dependencies_list}};

    fs::copy(template_root, project_root,
             fs::copy_options::recursive | fs::copy_options::skip_existing);
    for (auto& entry : fs::directory_iterator(template_root)) {
        searchAndReplace(project_root / entry.path().filename(), dictionnary);
    }
}

void TemplateManager::pImpl::createExecutable(
    const ExecutableParameters& executable,
    std::filesystem::path project_root) {
    createExecutableOrTest(executable, project_root,
                           templateRootPath() / "executable");
}

void TemplateManager::pImpl::createTest(const ExecutableParameters& test,
                                        std::filesystem::path project_root) {
    createExecutableOrTest(test, project_root, templateRootPath() / "test");
}

void TemplateManager::pImpl::createExecutableOrTest(
    const ExecutableParameters& params, std::filesystem::path project_root,
    std::filesystem::path template_root) {
    namespace fs = std::filesystem;

    const auto dependencies_list =
        fmt::format("{}", fmt::join(params.dependencies, "\n\t\t"));

    const auto dictionnary = std::map<std::string, std::string>{
        {"component_name", params.name},
        {"component_std", params.standard},
        {"component_dependencies", dependencies_list}};

    fs::copy(template_root, project_root,
             fs::copy_options::recursive | fs::copy_options::skip_existing);
    for (auto& entry : fs::directory_iterator(template_root)) {
        searchAndReplace(project_root / entry.path().filename(), dictionnary);
    }
}

bool TemplateManager::pImpl::downloadTemplate() {
    const auto filename = templateArchivePath().native();
    const auto url = templateUrl();

    auto curl = curl_easy_init();
    if (curl) {
        auto fp = fopen(filename.c_str(), "wb");
        if (fp == nullptr) {
            fmt::print(stderr, "Failed to open {} for writing\n", filename);
            curl_easy_cleanup(curl);
            return false;
        }

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

        auto res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fmt::print(stderr, "curl_easy_perform() failed: {}\n",
                       curl_easy_strerror(res));
            curl_easy_cleanup(curl);
            return false;
        }

        curl_easy_cleanup(curl);
        fclose(fp);

        return true;
    } else {
        fmt::print(stderr, "Failed to setup libcurl\n");
        return false;
    }
}

bool TemplateManager::pImpl::extractTemplate() {
    const auto filename = templateArchivePath().native();

    struct archive* a;
    struct archive* ext;
    struct archive_entry* entry;
    int r;

    int flags = ARCHIVE_EXTRACT_TIME;
    flags |= ARCHIVE_EXTRACT_PERM;
    flags |= ARCHIVE_EXTRACT_ACL;
    flags |= ARCHIVE_EXTRACT_FFLAGS;

    a = archive_read_new();
    ext = archive_write_disk_new();
    archive_write_disk_set_options(ext, flags);
    archive_read_support_format_tar(a);
    archive_read_support_filter_gzip(a);

    if ((r = archive_read_open_filename(a, filename.c_str(), 10240))) {
        fmt::print(stderr, "archive_read_open_filename(): {}\n",
                   archive_error_string(a));
        return false;
    }
    for (;;) {
        r = archive_read_next_header(a, &entry);
        if (r == ARCHIVE_EOF) {
            break;
        }
        if (r != ARCHIVE_OK) {
            fmt::print(stderr, "archive_read_next_header(): {}\n",
                       archive_error_string(a));
            return false;
        }
        r = archive_write_header(ext, entry);
        if (r != ARCHIVE_OK) {
            fmt::print(stderr, "archive_write_header(): {}\n",
                       archive_error_string(ext));
            return false;
        } else {
            copy_data(a, ext);
            r = archive_write_finish_entry(ext);
            if (r != ARCHIVE_OK) {
                fmt::print(stderr, "archive_write_finish_entry(): {}\n",
                           archive_error_string(ext));
                return false;
            }
        }
    }
    archive_read_close(a);
    archive_read_free(a);

    archive_write_close(ext);
    archive_write_free(ext);

    namespace fs = std::filesystem;
    fs::rename(fs::current_path() / "templates", templateRootPath());

    return true;
}

void TemplateManager::pImpl::searchAndReplace(
    std::filesystem::path root,
    std::map<std::string, std::string> dictionnary) const {
    namespace fs = std::filesystem;

    auto contain_pattern = [](const std::string& input) {
        std::regex r("__.*__");
        return std::regex_search(input, r);
    };

    auto replace_patterns = [&dictionnary](const std::string& input) {
        std::string output = input;
        for (const auto& [from, to] : dictionnary) {
            std::regex r(fmt::format("__{}__", from));
            output = std::regex_replace(output, r, to);
        }
        return output;
    };

    std::function<void(std::filesystem::path)> search_and_replace =
        [&](std::filesystem::path root) {
            for (auto& entry : fs::directory_iterator(root)) {
                const auto path = entry.path();
                const auto directory = entry.path().parent_path();
                auto filename = path.filename().native();
                if (contain_pattern(filename)) {
                    filename = replace_patterns(filename);
                    fs::rename(path, directory / filename);
                }
                const auto filepath = directory / filename;
                if (entry.is_regular_file()) {
                    std::ifstream input(filepath.native());
                    std::string content;
                    input.seekg(0, std::ios::end);
                    const auto size = input.tellg();
                    if (size > 0) {
                        content.reserve();
                        input.seekg(0, std::ios::beg);

                        content.assign((std::istreambuf_iterator<char>(input)),
                                       std::istreambuf_iterator<char>());
                        if (contain_pattern(content)) {
                            content = replace_patterns(content);
                            std::ofstream output(filepath.native());
                            output << content;
                        }
                    }
                } else if (entry.is_directory()) {
                    search_and_replace(filepath);
                }
            }
        };

    search_and_replace(root);
}

std::string TemplateManager::pImpl::templateUrl() const {
    return "https://github.com/BenjaminNavarro/cpgen/raw/master/share/"
           "templates.tar.gz";
}

std::filesystem::path TemplateManager::pImpl::templateArchivePath() const {
    return configRootPath() / "templates.tar.gz";
}

std::filesystem::path TemplateManager::pImpl::templateRootPath() const {
    return configRootPath() / "templates";
}

std::filesystem::path TemplateManager::pImpl::configRootPath() const {
    if (const char* home_path = std::getenv("HOME")) {
        auto path = std::filesystem::absolute(std::filesystem::path(home_path) /
                                              ".cpgen");
        std::filesystem::create_directories(path);
        return path;
    } else {
        throw std::runtime_error("Failed to get HOME environment variable");
    }
}

} // namespace cpgen