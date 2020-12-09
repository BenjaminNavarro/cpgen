add_library(common INTERFACE)

target_include_directories(common INTERFACE ${CMAKE_SOURCE_DIR}/include/common) 

target_compile_features(common INTERFACE cxx_std_11)