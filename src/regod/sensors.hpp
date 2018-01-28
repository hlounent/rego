/*
SPDX-License-Identifier: GPL-2.0-or-later

rego -- tools for interfacing with the Rego ground heat pump controller
Copyright (C) 2018  Hannu Lounento

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#pragma once

#include "fi/iki/halo/Rego1/Sensors/server.hpp"

#include <sdbusplus/server.hpp>

namespace rego {
namespace regod {

/**
 * Implements the D-Bus interface fi.iki.halo.Rego1.Sensors
 */
class sensors : public sdbusplus::server::object_t<sdbusplus::fi::iki::halo::Rego1::server::Sensors>
{
public:
    /**
     * Constructor
     *
     * @param bus The sdbusplus bus instance to be used
     * @param path The service object path
     * @param fd The Rego serial port file descriptor
     * @param timeout Timeout for reading from Rego
     */
    sensors(sdbusplus::bus::bus& bus, const char* path, int fd, int timeout);

    std::vector<std::tuple<std::string, sdbusplus::message::variant<bool, double>>> readAll() override;

private:
    int m_fd;
    int m_timeout;

};

} // namespace regod
} // namespace rego
