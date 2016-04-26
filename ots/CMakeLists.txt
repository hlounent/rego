# Compile 3rd party code as it is
set(CMAKE_CXX_FLAGS "-w")

if(BUILD_UNIT_TESTS OR BUILD_STEP_DEFINITIONS)
    add_subdirectory(gmock)
endif (BUILD_UNIT_TESTS OR BUILD_STEP_DEFINITIONS)
