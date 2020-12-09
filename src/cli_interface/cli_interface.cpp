#include "cli_interface_impl.h"
#include <cpgen/cli_interface.h>

#include <CLI/CLI.hpp>
#include <fmt/format.h>

#include <stdexcept>
#include <vector>

namespace cpgen {

CliInterface::CliInterface(int argc, char* argv[])
    : impl_{std::make_unique<pImpl>(argc, argv)} {
}

CliInterface::~CliInterface() = default;

const std::vector<LibraryParameters>& CliInterface::libraries() const {
    return impl().libraries();
}

const std::vector<ExecutableParameters>& CliInterface::executables() const {
    return impl().executables();
}

const std::vector<ExecutableParameters>& CliInterface::tests() const {
    return impl().tests();
}

const std::optional<ProjectParameters>& CliInterface::project() const {
    return impl().project();
}

const CliInterface::Options& CliInterface::options() const {
    return impl().options();
}

const CliInterface::pImpl& CliInterface::impl() const {
    return *impl_;
}

CliInterface::pImpl& CliInterface::impl() {
    return *impl_;
}

} // namespace cpgen