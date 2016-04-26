include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/ots/cucumber-cpp/cucumber-cpp/include)
include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/ots/gmock/gmock-1.7.0/gtest/include)
include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/ots/gmock/gmock-1.7.0/include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Werror -Wall -Wextra -Weffc++")
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything")
    # Pure virtual interfaces trigger -Wweak-vtables
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-weak-vtables")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-padded")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-global-constructors")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++98-compat")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra")
endif()

set(CucumberLibs cucumber-cpp boost_regex boost_system boost_thread boost_date_time)
set(GMockLibs gmock gmock_main)
set(RegoLibs protocol)

set(StepsSrc steps.cpp)
set(StepsBin steps)
set(StepsDeps ${RegoLibs} ${CucumberLibs} ${GMockLibs} -lstdc++ -lm)
add_executable(${StepsBin} ${StepsSrc})
target_link_libraries(${StepsBin} ${StepsDeps})

file(COPY cucumber.wire DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
