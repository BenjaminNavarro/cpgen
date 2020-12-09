#include <cpgen/cli_interface.h>

#include <CLI/CLI.hpp>

#include <vector>

namespace cpgen {

class CliInterface::pImpl {
public:
    pImpl(int argc, char* argv[]);

    const std::vector<LibraryParameters>& libraries() const;
    const std::vector<ExecutableParameters>& executables() const;
    const std::vector<ExecutableParameters>& tests() const;
    const std::optional<ProjectParameters>& project() const;
    const Options& options() const;

private:
    void addOptions();
    void createAddLibraryCommand();
    void onAddLibrary();
    void createAddExecutableCommand();
    void createAddTestCommand();
    void onAddExecutableOrTest(CLI::App* sub_command,
                               std::vector<ExecutableParameters>& add_to);
    void createNewProjectCommand();
    void onNewProject();

    CLI::App app_;

    std::vector<LibraryParameters> libraries_;
    std::vector<ExecutableParameters> executables_;
    std::vector<ExecutableParameters> tests_;
    std::optional<ProjectParameters> project_;
    CliInterface::Options options_;
};

} // namespace cpgen