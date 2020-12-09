#include <cpgen/cli_interface.h>
#include <cpgen/template_manager.h>

#include <fmt/format.h>

int main(int argc, char** argv) {
    cpgen::CliInterface cli{argc, argv};

    cpgen::TemplateManager template_manager;

    if (cli.options().update) {
        template_manager.update();
    }

    if (cli.project().has_value()) {
        template_manager.createProject(cli.project().value());
    }

    if (not cli.libraries().empty() or not cli.executables().empty() or
        not cli.tests().empty()) {
        namespace fs = std::filesystem;

        auto find_project_root = [] {
            auto current_dir = fs::current_path();

            do {
                if (fs::exists(current_dir / ".cpgen")) {
                    break;
                } else {
                    current_dir = current_dir.parent_path();
                }
            } while (current_dir != current_dir.root_directory());

            if (current_dir.parent_path() == current_dir) {
                throw std::runtime_error(
                    "Failed to locate a CPGen project root");
            }

            fmt::print("Project root path {}\n", current_dir.native());

            return current_dir;
        };

        auto project_root = cli.project().has_value()
                                ? fs::path(cli.project()->root_path) /
                                      fs::path(cli.project()->name)
                                : find_project_root();

        for (auto& library : cli.libraries()) {
            template_manager.createLibrary(library, project_root);
        }

        for (auto& executable : cli.executables()) {
            template_manager.createExecutable(executable, project_root);
        }

        for (auto& test : cli.tests()) {
            template_manager.createTest(test, project_root);
        }
    }
}
