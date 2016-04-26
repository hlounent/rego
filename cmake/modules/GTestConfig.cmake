set(GTestBinaryDir ${CMAKE_BINARY_DIR}/ots/gmock/gmock-1.7.0/gtest)

set(GTestLibrary ${GTestBinaryDir}/gtest)
set(GTestMainLibrary ${GTestBinaryDir}/gtest_main)
set(GTestIncludeDir ${CMAKE_SOURCE_DIR}/ots/gmock/gmock-1.7.0/gtest/include)

# Should use camelcase when searching for 'GTest' but cucumber expects capitals (GTEST_*)
#set(GTest_INCLUDE_DIRS ${GTestIncludeDir})
#set(GTest_LIBRARIES ${GTestLibrary} ${GTestMainLibrary})
#set(GTest_FOUND TRUE)

set(GTEST_INCLUDE_DIRS ${GTestIncludeDir})
set(GTEST_LIBRARY ${GTestLibrary})
set(GTEST_MAIN_LIBRARY ${GTestMainLibrary})
set(GTEST_LIBRARIES ${GTestLibrary} ${GTestMainLibrary})

set(GTEST_FOUND TRUE)
