#pragma once

#include <cpgen/common.h>

#include <filesystem>
#include <memory>

namespace cpgen {

class TemplateManager {
public:
    TemplateManager();
    ~TemplateManager(); // = default

    void update();

    void createProject(const ProjectParameters& project);

    void createLibrary(const LibraryParameters& library,
                       std::filesystem::path project_root);

    void createExecutable(const ExecutableParameters& executable,
                          std::filesystem::path project_root);

    void createTest(const ExecutableParameters& test,
                    std::filesystem::path project_root);

private:
    class pImpl;
    std::unique_ptr<pImpl> impl_;

    const pImpl& impl() const;
    pImpl& impl();
};

} // namespace cpgen