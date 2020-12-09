# Automatically downloads conan.cmake if not already present
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(DOWNLOAD   https://github.com/conan-io/cmake-conan/raw/v0.15/conan.cmake
                    ${CMAKE_BINARY_DIR}/conan.cmake)
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

if(ENABLE_TESTING)
    set(conan_build_tests True)
else()
    set(conan_build_tests False)
endif()

conan_cmake_run(
    CONANFILE conanfile.py
    BASIC_SETUP CMAKE_TARGETS
    BUILD missing
    OPTIONS ${PROJECT_NAME}:build_tests=${conan_build_tests}
)

set(conan_build_tests)