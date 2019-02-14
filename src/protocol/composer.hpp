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

#pragma once

#include "protocol/byte_sequence_t.hpp"

namespace rego {
namespace protocol {

/**
Formats data into Rego serial protocol packet format
*/
class composer
{
public:
    /** Constructor */
    composer() = default;

    /**
    Creates a message

    @param command [in] the operation to be requested
    @param parameter [in] parameter for the command
    @param value [in] the value if writing to a register, otherwise 0

    @return a message consisting of the given data, augmented with the heat pump
            address and checksu
    */
    byte_sequence_t create_message(unsigned char command, int16_t parameter, int16_t value);

    /**
    Calculates a checksum of the given data

    The checksum is an exclusive OR of all bytes.

    @param input [in] the data to be checksummed

    @return the checksum
    */
    unsigned char calculate_checksum(const byte_sequence_t& input);

    /**
    @return true if the checksum of the message is valid; false if not

    @param message [in] the message to be checked: the last byte is the checksum to be verified
    */
    bool is_checksum_valid(const byte_sequence_t& message);

private:
    composer(const composer&) = delete;
    composer& operator=(const composer&) = delete;

};

} // namespace protocol
} // namespace rego

