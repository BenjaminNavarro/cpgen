file(
    GLOB_RECURSE
    template_manager_FILES
    CONFIGURE_DEPENDS
    *.cpp
)

add_library(template_manager STATIC ${template_manager_FILES})

target_include_directories(template_manager PUBLIC ${CMAKE_SOURCE_DIR}/include/template_manager)

target_compile_features(template_manager PUBLIC cxx_std_20)

add_warnings(template_manager)

# Keep converage_config for coverage reports
target_link_libraries(template_manager 
    PUBLIC 
        coverage_config 
        common
        CONAN_PKG::libcurl
        CONAN_PKG::libarchive
        CONAN_PKG::fmt
        acl # for libarchive
)