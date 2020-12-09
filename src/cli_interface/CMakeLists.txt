file(
    GLOB_RECURSE
    cli_interface_FILES
    CONFIGURE_DEPENDS
    *.cpp
)

add_library(cli_interface STATIC ${cli_interface_FILES})

target_include_directories(cli_interface PUBLIC ${CMAKE_SOURCE_DIR}/include/cli_interface)

target_compile_features(cli_interface PUBLIC cxx_std_20)

add_warnings(cli_interface)

# Keep converage_config for coverage reports
target_link_libraries(cli_interface 
    PUBLIC 
        coverage_config
        common
        CONAN_PKG::cli11
        CONAN_PKG::fmt
)