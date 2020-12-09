add_library(__component_name__ INTERFACE)

target_include_directories(__component_name__ __component_type__ ${CMAKE_SOURCE_DIR}/include/__component_name__) 

target_compile_features(__component_name__ INTERFACE cxx_std___component_std__)

# Keep converage_config for coverage reports
target_link_libraries(__component_name__ 
    INTERFACE 
        coverage_config 
        __component_dependencies__
)