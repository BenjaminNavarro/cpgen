cmake_minimum_required(VERSION 3.10)

# Project description
project(
    __project_name__
    VERSION __project_version__
    DESCRIPTION "__project_description__"
    LANGUAGES CXX)

# Set the build type to release if not specified by the user    
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Use conan to gather dependencies
include(cmake/Conan.cmake)

# Add static analyzers support
include(cmake/StaticAnalyzers.cmake)

# Add code coverage support
include(cmake/Coverage.cmake)

# Add code coverage support
include(cmake/Doxygen.cmake)

# Add optional compiler options
include(cmake/CompilerOptions.cmake)

# Some useful CMake functions
include(cmake/CMakeUtils.cmake)

option(ENABLE_TESTING "Enable test compilation" OFF)
if(ENABLE_TESTING)
    enable_testing()
endif()

__cmake_pkgs__
# Process the src and apps directories
add_subdirectory(src)
add_subdirectory(apps)
add_subdirectory(tests)

# Force the generation of a compile_commands.json file to provide autocompletion for IDEs
include(cmake/CompileCommands.cmake)