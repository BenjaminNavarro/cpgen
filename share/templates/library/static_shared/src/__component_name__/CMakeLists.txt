file(
    GLOB_RECURSE
    __component_name___FILES
    CONFIGURE_DEPENDS
    *.cpp
)

add_library(__component_name__ __component_type__ ${__component_name___FILES})

target_include_directories(__component_name__ PUBLIC ${CMAKE_SOURCE_DIR}/include/__component_name__)

target_compile_features(__component_name__ PUBLIC cxx_std___component_std__)

add_warnings(__component_name__)

# Keep converage_config for coverage reports
target_link_libraries(__component_name__ 
    PUBLIC 
        coverage_config 
        __component_dependencies__
)