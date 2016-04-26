find_package(Boost 1.54 COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

add_executable(marshaller marshaller.cpp)
target_link_libraries(marshaller ${Boost_LIBRARIES})
target_link_libraries(marshaller protocol)
