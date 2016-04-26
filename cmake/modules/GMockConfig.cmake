set(GMockBinaryDir ${CMAKE_BINARY_DIR}/ots/gmock/gmock-1.7.0)

set(GMockLibrary ${GMockBinaryDir}/gmock)
set(GMockMainLibrary ${GMockBinaryDir}/gmock_main)
set(GMockIncludeDir ${CMAKE_SOURCE_DIR}/ots/gmock/gmock-1.7.0/include)

set(GMock_INCLUDE_DIRS ${GMockIncludeDir})
set(GMock_LIBRARIES ${GMockLibrary} ${GMockMainLibrary})
set(GMock_FOUND TRUE)
