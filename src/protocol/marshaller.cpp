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

#include "protocol/marshaller.hpp"

namespace rego {
namespace protocol {

marshaller::marshaller()
{
}

marshaller::~marshaller()
{
}

byte_sequence_t marshaller::marshall(int16_t input)
{
    byte_sequence_t output;
    output.push_back((input >> 14) & 0x03);
    output.push_back((input >> 7) & 0x7F);
    output.push_back(input & 0x7F);

    return output;
}

int16_t marshaller::unmarshall(const byte_sequence_t& input)
{
    int16_t result = 0;
    byte_sequence_t::const_reverse_iterator it = input.rbegin();
    const byte_sequence_t::const_reverse_iterator end = input.rend();
    int i = 0;
    for (; it != end; ++it, ++i)
    {
        const uint8_t part = *it;
        result |= (part << i * 7);
    }
    return result;
}

} // namespace protocol
} // namespace rego
