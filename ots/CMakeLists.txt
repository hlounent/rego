# Compile 3rd party code as it is
set(CMAKE_CXX_FLAGS "-w")

if(BUILD_UNIT_TESTS OR BUILD_STEP_DEFINITIONS)
    add_subdirectory(gmock)
endif (BUILD_UNIT_TESTS OR BUILD_STEP_DEFINITIONS)

if(BUILD_STEP_DEFINITIONS)
    add_subdirectory(cucumber-cpp)
endif(BUILD_STEP_DEFINITIONS)
