/*
SPDX-License-Identifier: GPL-2.0-or-later

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

#include "protocol/composer.hpp"

#include "protocol/marshaller.hpp"

namespace rego {
namespace protocol {

namespace {

// Constants
enum
{
    // The address of the heat pump is always 0x81
    HEAT_PUMP_ADDRESS = 0x81
};

} // unnamed namespace

composer::composer()
{
}

composer::~composer()
{
}

byte_sequence_t composer::create_message(unsigned char command, int16_t parameter, int16_t value)
{
    byte_sequence_t message;
    message.append(1, HEAT_PUMP_ADDRESS);
    message.append(1, command);

    marshaller m;
    byte_sequence_t data;
    data.append(m.marshall(parameter));
    data.append(m.marshall(value));
    message.append(data);

    // Checksum only includes the parameter and value,
    // and excludes the address and command
    message.append(1, calculate_checksum(data));

    return message;
}

unsigned char composer::calculate_checksum(const byte_sequence_t& input)
{
    unsigned char result = 0;
    for (const unsigned char& c : input)
    {
        result ^= c;
    }
    return result;
}

bool composer::is_checksum_valid(const byte_sequence_t& message)
{
    const byte_sequence_t payload(message.begin(), message.end() - 1);
    const unsigned char checksum = calculate_checksum(payload);

    return message.back() == checksum;
}

} // namespace protocol
} // namespace rego

