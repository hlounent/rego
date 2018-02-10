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

#include "protocol/byte_sequence_t.hpp"

#include <boost/optional.hpp>
#include <cstdint>
#include <string>
#include <termios.h>

namespace rego {
namespace util {

/**
Opens the given device node as a serial port

@param path [in] the path of the device (e.g. /dev/ttyUSB0)
@param oldtio [out] storage for saving port's current configuration
*/
int open_serial_port(const std::string& path, struct termios& oldtio);

/**
Closes the serial port associated with the given fd restoring it to the given configuration

@param fd [in] The file descriptor of the port to be closed
@param oldtio [in] The configuration to be restored
*/
void close_serial_port(int fd, const struct termios& oldtio);

/**
Reads a response from Rego

@param fd [in] the file descriptor to read from
@param timeout [in] timeout (in seconds) for read
@param response [out] the buffer to write the received data to

Note: Due to poll, which does not decrement the timeout value like select does,
      the total timeout may be longer than the given value

@todo Measure time spent in poll and decrement the timeout value for subsequent calls
*/
unsigned int read_response(int fd, int timeout, protocol::byte_sequence_t& response);

/**
Handles a response received from Rego

@param response [in] the response
@param length [in] the length of the response, i.e. how many bytes 'response' contains
*/
boost::optional<int16_t> handle_response(const protocol::byte_sequence_t& response, unsigned int length);

/**
Reads a single register and handles the response

@param fd [in] the file descriptor to read from
@param address [in] the address of the register
@param timeout [in] timeout (in seconds) for read
*/
boost::optional<int16_t> read_register(int fd, int16_t address, int timeout);

/**
Writes a value to a single register

@param fd [in] the file descriptor to write to
@param address [in] the address of the register
@param value [in] the value to be written
@param timeout [in] timeout (in seconds) for reading the response
*/
void write_register(int fd, int16_t address, int16_t value, int timeout);

} // namespace util
} // namespace rego
