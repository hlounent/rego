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

#pragma once

#include "protocol/byte_sequence_t.hpp"

#include <cstdint>

namespace rego {
namespace protocol {

/**
marshaller converts data to and from the format required by the serial protocol

The serial protocol data format is 7 bits per byte, highest byte first.
*/
class marshaller
{
public:
    /** Constructor */
    marshaller();

    /**  Destructor */
    virtual ~marshaller();

    /**
    Converts data to the serial protocol format

    @param input [in] the data to be converted

    @return the converted data
    */
    byte_sequence_t marshall(int16_t input);

    /**
    Converts data from the serial protocol format

    @param input [in] the data to be converted

    @return the converted data
    */
    int16_t unmarshall(const byte_sequence_t& input);

private:
    marshaller(const marshaller&) = delete;
    marshaller& operator=(const marshaller&) = delete;

};

} // namespace protocol
} // namespace rego
