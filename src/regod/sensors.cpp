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

#include "regod/sensors.hpp"

#include "util/serial.hpp"

#include <boost/optional.hpp>
#include <log4cxx/logger.h>
#include <vector>

using namespace log4cxx;
using namespace rego::util;

namespace rego {
namespace regod {

namespace {

LoggerPtr logger(Logger::getLogger("regod.sensors"));

} // anonymous namepace

class conversion
{
public:
    virtual ~conversion() = default;
    virtual sdbusplus::message::variant<bool, double> convert(int16_t value) = 0;
};

class conversion_to_bool : public conversion
{
public:
    sdbusplus::message::variant<bool, double> convert(const int16_t value) override
    {
        return sdbusplus::message::variant<bool, double>(static_cast<bool>(value));
    }
};
conversion_to_bool conversion_to_bool;

class conversion_to_double : public conversion
{
public:
    sdbusplus::message::variant<bool, double> convert(const int16_t value) override
    {
        return sdbusplus::message::variant<bool, double>(static_cast<double>(value) / 10.0);
    }
};
conversion_to_double conversion_to_double;

struct register_t
{
public:
    const char* const m_name;
    const int m_address;
    conversion& m_conversion;
} registers[] =
{
    { "GT1 Radiator return",        0x209,    conversion_to_double },
    { "GT2 Outdoor",                0x20a,    conversion_to_double },
    { "GT3 Hot water",              0x20b,    conversion_to_double },
    { "GT4 Forward",                0x20c,    conversion_to_double },
    { "GT5 Room",                   0x20d,    conversion_to_double },
    { "GT6 Hot gas",                0x20e,    conversion_to_double },
    { "GT8 Heat fluid out",         0x20f,    conversion_to_double },
    { "GT9 Heat fluid in",          0x210,    conversion_to_double },
    { "GT10 Cold fluid in",         0x211,    conversion_to_double },
    { "GT11 Cold fluid out",        0x212,    conversion_to_double },
    { "GT3x External hot water",    0x213,    conversion_to_double },
    { "GT1 Target value",            0x6e,    conversion_to_double },
    { "GT1 On value",                0x6f,    conversion_to_double },
    { "GT1 Off value",               0x70,    conversion_to_double },
    { "GT3 Target value",            0x2b,    conversion_to_double },
    { "GT3 On value",                0x73,    conversion_to_double },
    { "GT3 Off value",               0x74,    conversion_to_double },
    { "P3 Cold fluid pump",         0x1fd,      conversion_to_bool },
    { "Compressor",                 0x1fe,      conversion_to_bool },
    { "Additional heat 1",          0x1ff,      conversion_to_bool },
    { "Additional heat 2",          0x200,      conversion_to_bool },
    { "P1 Radiator pump",           0x203,      conversion_to_bool },
    { "P2 Heat fluid pump",         0x204,      conversion_to_bool },
    { "Three-way valve",            0x205,      conversion_to_bool },
    { "Alarm",                      0x206,      conversion_to_bool },
};

sensors::sensors(sdbusplus::bus::bus& bus, const char* path, int fd, int timeout)
  : sdbusplus::server::object_t<sdbusplus::fi::iki::halo::Rego1::server::Sensors>(bus, path)
  , m_fd(fd)
  , m_timeout(timeout)
{
}

std::vector<std::tuple<std::string, sdbusplus::message::variant<bool, double>>> sensors::readAll()
{
    std::vector<std::tuple<std::string, sdbusplus::message::variant<bool, double>>> results;
    LOG4CXX_TRACE(logger, "Reading registers");
    for (const auto reg : registers)
    {
        const boost::optional<int16_t> value =
            read_register(m_fd, static_cast<int16_t>(reg.m_address), m_timeout);
        if (value)
        {
            LOG4CXX_TRACE(logger, "Register " << reg.m_name << ": " << *value);
            results.push_back(
                std::make_tuple<std::string, sdbusplus::message::variant<bool, double>>(
                    reg.m_name,
                    reg.m_conversion.convert(*value)));
        }
        else
        {
            LOG4CXX_DEBUG(logger, "Register " << reg.m_name << " value missing");
        }
    }
    return results;
}

} // namespace regod
} // namespace rego
