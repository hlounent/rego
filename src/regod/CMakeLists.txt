# SPDX-License-Identifier: GPL-2.0-or-later

find_package(Boost 1.54 COMPONENTS program_options system REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

# @todo Fix and use the check
#set(THREADS_PREFER_PTHREAD_FLAG ON)
#find_package(Threads REQUIRED)
#    Threads::Threads

pkg_check_modules(sdbusplus REQUIRED sdbusplus)
include_directories(${sdbusplus_INCLUDE_DIR})

# For sdbusc++ generated server.cpp
include_directories(.)

set(src
    fi/iki/halo/Rego1/Sensors/server.cpp
    regod.cpp
    sensors.cpp
)

add_executable(regod ${src})
target_link_libraries(regod
    protocol
    util
    ${Boost_LIBRARIES}
    ${log4cxx_LIBRARIES}
    ${sdbusplus_LIBRARIES}
    pthread
)

install(TARGETS regod DESTINATION ${CMAKE_INSTALL_SBINDIR})
