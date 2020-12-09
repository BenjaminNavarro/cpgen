#pragma once

#include <cpgen/common.h>

#include <memory>
#include <optional>
#include <string_view>
#include <vector>

namespace cpgen {

class CliInterface {
public:
    struct Options {
        bool update{false};
    };

    CliInterface(int argc, char* argv[]);
    ~CliInterface(); // = default

    const std::vector<LibraryParameters>& libraries() const;
    const std::vector<ExecutableParameters>& executables() const;
    const std::vector<ExecutableParameters>& tests() const;
    const std::optional<ProjectParameters>& project() const;
    const Options& options() const;

private:
    class pImpl;
    std::unique_ptr<pImpl> impl_;

    const pImpl& impl() const;
    pImpl& impl();
};

} // namespace cpgen