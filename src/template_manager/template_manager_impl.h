#include <cpgen/template_manager.h>

#include <cstddef>
#include <map>

namespace cpgen {

class TemplateManager::pImpl {
public:
    pImpl();
    ~pImpl();

    bool update();

    void createProject(const ProjectParameters& project);

    void createLibrary(const LibraryParameters& library,
                       std::filesystem::path project_root);

    void createExecutable(const ExecutableParameters& executable,
                          std::filesystem::path project_root);

    void createTest(const ExecutableParameters& test,
                    std::filesystem::path project_root);

private:
    bool downloadTemplate();
    bool extractTemplate();

    void createExecutableOrTest(const ExecutableParameters& test,
                                std::filesystem::path project_root,
                                std::filesystem::path template_root);

    void searchAndReplace(std::filesystem::path root,
                          std::map<std::string, std::string> dictionnary) const;

    std::string templateUrl() const;
    std::filesystem::path templateArchivePath() const;
    std::filesystem::path configRootPath() const;
    std::filesystem::path templateRootPath() const;
};

} // namespace cpgen