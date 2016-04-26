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

namespace rego {
namespace protocol {

/**
Creates known Rego serial protocol messages
*/
class rego_638
{
public:
    /** Constructor */
    rego_638();

    /** Destructor */
    virtual ~rego_638();

    /** Composes the version query message */
    byte_sequence_t version_query() const;

    /**
    Composes a message for reading the given sensor

    @param sensor [in] the name of the sensor

    @return the sensor query message
    */
    byte_sequence_t sensor_query(const std::string& sensor) const;

private:
    rego_638(const rego_638&) = delete;
    rego_638& operator=(const rego_638&) = delete;

};

} // namespace protocol
} // namespace rego

