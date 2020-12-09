set(cpgen_files 
    main.cpp
)

add_executable(cpgen ${cpgen_files})

target_compile_features(cpgen PRIVATE cxx_std_20)

target_link_libraries(cpgen PRIVATE
    cli_interface
    template_manager
    CONAN_PKG::fmt
)

add_warnings(cpgen)