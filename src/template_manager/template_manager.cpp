#include "template_manager_impl.h"
#include <cpgen/template_manager.h>

#include <stdexcept>
#include <vector>

namespace cpgen {

TemplateManager::TemplateManager() : impl_{std::make_unique<pImpl>()} {
}

TemplateManager::~TemplateManager() = default;

void TemplateManager::update() {
    impl().update();
}

void TemplateManager::createProject(const ProjectParameters& project) {
    impl().createProject(project);
}

void TemplateManager::createLibrary(const LibraryParameters& library,
                                    std::filesystem::path project_root) {
    impl().createLibrary(library, project_root);
}

void TemplateManager::createExecutable(const ExecutableParameters& executable,
                                       std::filesystem::path project_root) {
    impl().createExecutable(executable, project_root);
}

void TemplateManager::createTest(const ExecutableParameters& test,
                                 std::filesystem::path project_root) {
    impl().createTest(test, project_root);
}

const TemplateManager::pImpl& TemplateManager::impl() const {
    return *impl_;
}

TemplateManager::pImpl& TemplateManager::impl() {
    return *impl_;
}

} // namespace cpgen