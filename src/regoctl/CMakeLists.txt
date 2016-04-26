find_package(Boost 1.54 COMPONENTS program_options system REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

# @todo Fix and use the check
#set(THREADS_PREFER_PTHREAD_FLAG ON)
#find_package(Threads REQUIRED)
#    Threads::Threads

add_executable(regoctl regoctl.cpp)
target_link_libraries(regoctl
    protocol
    ${Boost_LIBRARIES}
    pthread
)

install(TARGETS regoctl DESTINATION bin)
