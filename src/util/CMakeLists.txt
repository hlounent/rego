# SPDX-License-Identifier: GPL-2.0-or-later

set(src
    helpers.cpp
    serial.cpp
    )

add_library(util STATIC ${src})
target_link_libraries(util
    protocol
)
