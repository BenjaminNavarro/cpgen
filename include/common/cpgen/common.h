#pragma once

#include <string>
#include <vector>

namespace cpgen {

enum class LibraryType { Static, Shared, HeaderOnly, Module };
struct LibraryParameters {
    std::string name{};
    LibraryType type{LibraryType::Static};
    std::vector<std::string> dependencies;
    std::string standard{"11"};
};

struct ExecutableParameters {
    std::string name{};
    std::vector<std::string> dependencies;
    std::string standard{"11"};
};

struct ProjectParameters {
    std::string name{};
    std::string version{"0.1.0"};
    std::string description{""};
    std::vector<std::string> conan_pkgs;
    std::vector<std::string> cmake_pkgs;
    std::string root_path{"."};
};

} // namespace cpgen