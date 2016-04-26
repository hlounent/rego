cmake_minimum_required(VERSION 3.2)
project(
    rego
    VERSION 0.0.1
)

option(BUILD_UNIT_TESTS "Build C++ unit tests" ON)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules/")

if(BUILD_UNIT_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif(BUILD_UNIT_TESTS)

add_subdirectory(ots)
add_subdirectory(src)
