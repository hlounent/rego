project(cucumber-cpp)

set(CUKE_USE_STATIC_BOOST ON CACHE BOOL "Statically link Boost (except boost::test)")
set(CUKE_DISABLE_BOOST_TEST ON CACHE BOOL "Disable boost:test")
set(CUKE_DISABLE_CPPSPEC ON CACHE BOOL "Disable CppSpec")
set(CUKE_DISABLE_GTEST OFF CACHE BOOL "Disable Google Test framework")
set(CUKE_DISABLE_FUNCTIONAL ON CACHE BOOL "Disable Functional Tests")
set(CUKE_ENABLE_EXAMPLES OFF CACHE BOOL "Enable the examples")

#set(GTest_DIR ${CMAKE_SOURCE_DIR}/cots/gmock)
#set(GMock_DIR ${CMAKE_SOURCE_DIR}/cots/gmock)
#set(GTEST_ROOT ${CMAKE_SOURCE_DIR}/cots/gmock/gmock-1.7.0/gtest)
#set(GMOCK_ROOT ${CMAKE_SOURCE_DIR}/cots/gmock/gmock-1.7.0)

add_subdirectory(cucumber-cpp)
