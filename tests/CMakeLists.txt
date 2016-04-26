include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/ots/gmock/gmock-1.7.0/gtest/include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Werror -Wall -Wextra -Weffc++")

set (UNIT_TEST_SRC
    protocol/test_composer.cpp
)

set(GTestLibs gtest gtest_main)

add_executable(unit_test_runner ${UNIT_TEST_SRC})
target_link_libraries(unit_test_runner
    protocol
    ${GTestLibs}
)

add_test(AllTests unit_test_runner)
