#include "cli_interface_impl.h"

#include <CLI/CLI.hpp>
#include <fmt/format.h>

#include <stdexcept>
#include <vector>

namespace {

void set_if(CLI::Option* option, auto& value) {
    if (option->count() > 0) {
        value = option->as<std::remove_reference_t<decltype(value)>>();
    }
}

} // namespace

namespace cpgen {

CliInterface::pImpl::pImpl(int argc, char* argv[])
    : app_{"CPGen - The C++ project generator"} {
    addOptions();
    createAddLibraryCommand();
    createAddExecutableCommand();
    createAddTestCommand();
    createNewProjectCommand();

    app_.callback([&] {
        if (app_.count_all() == 1) {
            fmt::print(app_.help());
        }
    });

    try {
        app_.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
        app_.exit(e);
    }
}

const std::vector<LibraryParameters>& CliInterface::pImpl::libraries() const {
    return libraries_;
}

const std::vector<ExecutableParameters>&
CliInterface::pImpl::executables() const {
    return executables_;
}

const std::vector<ExecutableParameters>& CliInterface::pImpl::tests() const {
    return tests_;
}

const std::optional<ProjectParameters>& CliInterface::pImpl::project() const {
    return project_;
}

const CliInterface::Options& CliInterface::pImpl::options() const {
    return options_;
}

void CliInterface::pImpl::addOptions() {
    auto update = app_.add_subcommand("update", "Update the templates");
    update->callback([&] { options_.update = true; });
    // app_.add_flag("--update", options_.update, "Update the templates");
}

void CliInterface::pImpl::createAddLibraryCommand() {
    auto add_library =
        app_.add_subcommand("add-library", "Add a new library to the project");

    add_library->immediate_callback();
    add_library->parse_complete_callback([this]() { onAddLibrary(); });

    auto name = add_library->add_option("--name", "The library name");

    auto type = add_library->add_option("--type", "The library type");

    auto std = add_library->add_option(
        "--std", "The standard library version to use (e.g 11 for C++11)");

    auto deps =
        add_library->add_option("--dependencies", "The library dependencies")
            ->take_all()
            ->allow_extra_args(true);

    add_library->needs(name);
    type->needs(name);
    type->check(CLI::IsMember({"static", "shared", "header_only", "module"}));
    std->needs(name);
    deps->needs(name);
}

void CliInterface::pImpl::onAddLibrary() {
    LibraryParameters params;

    auto add_library = app_.get_subcommand("add-library");

    params.name = add_library->get_option("--name")->as<std::string>();

    params.type = [&](const auto& type) {
        if (type == "static") {
            return LibraryType::Static;
        } else if (type == "shared") {
            return LibraryType::Shared;
        } else if (type == "header_only") {
            return LibraryType::HeaderOnly;
        } else if (type == "module") {
            return LibraryType::Module;
        } else if (type.empty()) {
            return params.type;
        } else {
            throw std::invalid_argument("Invalid library type");
        }
    }(add_library->get_option("--type")->as<std::string>());

    params.dependencies = add_library->get_option("--dependencies")
                              ->as<std::vector<std::string>>();

    set_if(add_library->get_option("--std"), params.standard);

    libraries_.emplace_back(params);
}

void CliInterface::pImpl::createAddExecutableCommand() {
    auto add_exe = app_.add_subcommand("add-executable",
                                       "Add a new executable to the project");

    add_exe->immediate_callback();
    add_exe->parse_complete_callback(
        [this, add_exe]() { onAddExecutableOrTest(add_exe, executables_); });

    auto name = add_exe->add_option("--name", "The executable name");

    auto std = add_exe->add_option(
        "--std", "The standard library version to use (e.g 11 for C++11)");

    auto deps =
        add_exe->add_option("--dependencies", "The executable dependencies")
            ->take_all()
            ->allow_extra_args(true);

    add_exe->needs(name);
    std->needs(name);
    deps->needs(name);
}

void CliInterface::pImpl::createAddTestCommand() {
    auto add_test =
        app_.add_subcommand("add-test", "Add a new test to the project");

    add_test->immediate_callback();
    add_test->parse_complete_callback(
        [this, add_test]() { onAddExecutableOrTest(add_test, tests_); });

    auto name = add_test->add_option("--name", "The test name");

    auto std = add_test->add_option(
        "--std", "The standard library version to use (e.g 11 for C++11)");

    auto deps = add_test->add_option("--dependencies", "The test dependencies")
                    ->take_all()
                    ->allow_extra_args(true);

    add_test->needs(name);
    std->needs(name);
    deps->needs(name);
}

void CliInterface::pImpl::onAddExecutableOrTest(
    CLI::App* sub_command, std::vector<ExecutableParameters>& add_to) {
    ExecutableParameters params;

    params.name = sub_command->get_option("--name")->as<std::string>();

    set_if(sub_command->get_option("--std"), params.standard);

    params.dependencies = sub_command->get_option("--dependencies")
                              ->as<std::vector<std::string>>();

    add_to.emplace_back(params);
}

void CliInterface::pImpl::createNewProjectCommand() {
    auto new_project =
        app_.add_subcommand("new-project", "Create a new project");

    // at most one project per invocation
    new_project->require_subcommand(-1);

    new_project->immediate_callback();
    new_project->parse_complete_callback([this]() { onNewProject(); });

    auto name = new_project->add_option("--name", "The project name");
    auto version = new_project->add_option("--version", "The project version");
    auto description =
        new_project->add_option("--description", "The project description");

    auto conan_pkgs =
        new_project->add_option("--conan-pkgs", "The required Conan packages")
            ->take_all()
            ->allow_extra_args(true);

    auto cmake_pkgs =
        new_project->add_option("--cmake-pkgs", "The required CMake packages")
            ->take_all()
            ->allow_extra_args(true);

    auto root_path = new_project->add_option("--root", "The project root path");

    new_project->needs(name);
    version->needs(name);
    description->needs(name);
    conan_pkgs->needs(name);
    cmake_pkgs->needs(name);
    root_path->needs(name);
}

void CliInterface::pImpl::onNewProject() {
    ProjectParameters params;

    auto add_project = app_.get_subcommand("new-project");

    params.name = add_project->get_option("--name")->as<std::string>();
    set_if(add_project->get_option("--version"), params.version);
    set_if(add_project->get_option("--description"), params.description);
    set_if(add_project->get_option("--root"), params.root_path);

    params.conan_pkgs =
        add_project->get_option("--conan-pkgs")->as<std::vector<std::string>>();
    params.cmake_pkgs =
        add_project->get_option("--cmake-pkgs")->as<std::vector<std::string>>();

    project_ = params;
}

} // namespace cpgen