# SPDX-License-Identifier: GPL-2.0-or-later

set(src
    composer.cpp
    marshaller.cpp
    rego_638.cpp
    )

add_library(protocol STATIC ${src})
