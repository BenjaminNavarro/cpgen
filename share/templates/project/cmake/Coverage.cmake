# Adapted from https://github.com/codecov/example-cpp11-cmake

add_library(coverage_config INTERFACE)

option(ENABLE_LCOV "Enable coverage reporting using lcov" OFF)
if(ENABLE_LCOV AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  find_program(LCOV lcov)
  if(LCOV)
    message("lcov finished setting up.")
  else()
    message(SEND_ERROR "lcov requested but executable not found.")
  endif()

  find_program(GENHTML genhtml)
  if(GENHTML)
    set(GENHTML_ARGS "-o;coverage;coverage.info")
  else()
    message("genhtml not found in PATH, disabling HTML coverage report generation")
  endif()

  # Add required flags (GCC & LLVM/Clang)
  target_compile_options(coverage_config INTERFACE
    -O0        # no optimization
    -g         # generate debug info
    --coverage # sets all required flags
  )
  
  if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.13)
    target_link_options(coverage_config INTERFACE --coverage)
  else()
    target_link_libraries(coverage_config INTERFACE --coverage)
  endif()

  add_custom_target(
    coverage
    COMMAND ctest
    COMMAND ${LCOV} --capture --directory . --output-file coverage.info
    COMMAND ${LCOV} --remove coverage.info '/usr/*' --output-file coverage.info
    COMMAND ${LCOV} --remove coverage.info '$ENV{HOME}/.conan/*' --output-file coverage.info
    COMMAND ${LCOV} --remove coverage.info '${CMAKE_SOURCE_DIR}/tests/*' --output-file coverage.info
    COMMAND ${LCOV} --list coverage.info
    COMMAND ${GENHTML} ${GENHTML_ARGS}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  )
endif()
