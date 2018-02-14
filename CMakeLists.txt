# SPDX-License-Identifier: GPL-2.0-or-later

cmake_minimum_required(VERSION 3.2)
project(
    rego
    VERSION 0.0.1
)

set(CMAKE_CXX_STANDARD 14)

option(BUILD_UNIT_TESTS "Build C++ unit tests" ON)
option(BUILD_STEP_DEFINITIONS "Build Gherkin C++ step definitions" ON)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules/")

if(BUILD_UNIT_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif(BUILD_UNIT_TESTS)

if(BUILD_STEP_DEFINITIONS)
    add_subdirectory(features)
endif(BUILD_STEP_DEFINITIONS)

add_subdirectory(ots)
add_subdirectory(src)
