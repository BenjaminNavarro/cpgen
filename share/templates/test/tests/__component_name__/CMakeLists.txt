file(
    GLOB_RECURSE
    __component_name___FILES
    CONFIGURE_DEPENDS
    *.cpp
)

add_executable(__component_name__-test ${__component_name___FILES})

target_compile_features(__component_name__-test PRIVATE cxx_std___component_std__)

target_link_libraries(__component_name__-test
    PRIVATE
        __component_dependencies__
)

add_warnings(__component_name__-test)

add_test(NAME __component_name__-test COMMAND __component_name__-test)
