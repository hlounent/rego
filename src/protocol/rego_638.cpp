/*
rego -- tools for interfacing with the Rego ground heat pump controller
Copyright (C) 2016  Hannu Lounento

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

#include "protocol/rego_638.hpp"

#include "protocol/composer.hpp"
#include "protocol/marshaller.hpp"

#include <map>

namespace rego {
namespace protocol {

namespace {

// Known commands supported by Rego
enum command
{
    // Version query command
    VERSION_QUERY_COMMAND = 0x7f,
    // Requests a sensor value
    SENSOR_READ_COMMAND = 0x02
};

/*
@return the address of the given sensor

@param sensor [in] the name of the sensor
*/
int16_t get_sensor_id(const std::string& sensor)
{
    typedef std::map<std::string, int16_t> sensor_map;
    const sensor_map sensors
    {
        { "GT1", 0x020b },
        { "GT2", 0x020c },
        { "GT3", 0x020d },
        { "GT4", 0x020e },
        { "GT5", 0x020f },
        { "GT6", 0x0210 },
        { "GT8", 0x0211 },
        { "GT9", 0x0212 },
        { "GT10", 0x0213 },
        { "GT11", 0x0214 },
        { "GT3x", 0x0215 }
    };
    return sensors.at(sensor);
}

} // unnamed namespace

rego_638::rego_638()
{
}

rego_638::~rego_638()
{
}

byte_sequence_t rego_638::version_query() const
{
    composer c;
    return c.create_message(
        VERSION_QUERY_COMMAND,
        0x00,
        0x00);
}

byte_sequence_t rego_638::sensor_query(const std::string& sensor) const
{
    composer c;
    return c.create_message(
        SENSOR_READ_COMMAND,
        get_sensor_id(sensor),
        0x00);
}

} // namespace protocol
} // namespace rego

