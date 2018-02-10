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

// POSIX compliant source
#define _POSIX_SOURCE 1

#include "protocol/composer.hpp"
#include "protocol/marshaller.hpp"
#include "util/helpers.hpp"
#include "util/serial.hpp"

#include <cassert>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <log4cxx/logger.h>
#include <poll.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

using namespace log4cxx;

namespace rego {
namespace util {

namespace {

// The response packet is always 5 bytes long
constexpr int RESPONSE_LENGTH = 5;

LoggerPtr logger(Logger::getLogger("util.serial"));

} // anonymous namespace

int open_serial_port(const std::string& path, struct termios& oldtio)
{
    const int fd = open(path.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd < 0)
    {
        return EXIT_FAILURE;
    }

    tcgetattr(fd, &oldtio);

    // @todo Replace asserts with non-fatal checks
    struct termios newtio;
    memset(&newtio, 0, sizeof(newtio));
    newtio.c_cflag = CS8 | CRTSCTS | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
//    newtio.c_iflag = 0;
    newtio.c_oflag = 0;
    newtio.c_lflag = 0; // Non-canonical mode, i.e. don't modify the data
    newtio.c_cc[VTIME] = 0; // Don't block
    newtio.c_cc[VMIN] = 0; // Read only what is available
    cfmakeraw(&newtio);
    assert(cfsetospeed(&newtio, B19200) == 0);
    assert(cfsetispeed(&newtio, B19200) == 0); // @todo This could be set to zero to match the output baud rate

    tcflush(fd, TCIFLUSH);
    tcsetattr(fd, TCSANOW, &newtio);

    // Check that the settings are in effect
    {
        struct termios currenttio;
        tcgetattr(fd, &currenttio);
        assert(currenttio.c_cflag == newtio.c_cflag);
        assert(currenttio.c_iflag == newtio.c_iflag);
        assert(currenttio.c_oflag == newtio.c_oflag);
        assert(currenttio.c_lflag == newtio.c_lflag);
    }

    return fd;
}

void close_serial_port(const int fd, const struct termios& oldtio)
{
    tcsetattr(fd, TCSANOW, &oldtio);
    close(fd);
}

unsigned int read_response(const int fd, const int timeout, protocol::byte_sequence_t& response)
{
    unsigned int bytes_received = 0;

    static constexpr int N_FDS = 1;
    struct pollfd fds[N_FDS];
    fds[0].fd = fd;
    fds[0].events = POLLIN;
    fds[0].revents = 0;

    while (bytes_received < RESPONSE_LENGTH)
    {
        const int rv = poll(fds, N_FDS, timeout);
        if (rv == -1)
        {
            LOG4CXX_ERROR(logger, "Failed to read response" << strerror(errno));
            break;
        }
        else if (rv == 0)
        {
            LOG4CXX_WARN(logger, "Time-out!");
            break;
        }
        else
        {
            static constexpr int BUFFER_LENGTH = 10;
            unsigned char read_buffer[BUFFER_LENGTH];
            const ssize_t received = read(fd, read_buffer, BUFFER_LENGTH);
            read_buffer[received] = 0;

            LOG4CXX_DEBUG(logger, "Read " << received << " bytes");

            response.append(read_buffer, static_cast<protocol::byte_sequence_t::size_type>(received));
            bytes_received += received;
        }
    }
    return bytes_received;
}

boost::optional<int16_t> handle_response(const protocol::byte_sequence_t& response, const unsigned int length)
{
    if (length == 0)
    {
        LOG4CXX_WARN(logger, "No response received");
    }
    else if (length > 0 && length < RESPONSE_LENGTH)
    {
        LOG4CXX_WARN(logger, "Invalid message: too short message: " << length);
    }
    else if (length > RESPONSE_LENGTH)
    {
        LOG4CXX_WARN(logger, "Invalid message: too long message: " << length);
    }
    else if (length == RESPONSE_LENGTH)
    {
        protocol::composer c;
        if (response.at(0) != 0x01)
        {
            LOG4CXX_WARN(logger, "Invalid message: incorrect destination address");
        }
        else if (!c.is_checksum_valid(
            protocol::byte_sequence_t(response.begin() + 1, response.begin() + RESPONSE_LENGTH)))
        {
            LOG4CXX_WARN(logger, "Invalid message: incorrect checksum");
        }
        else
        {
            protocol::byte_sequence_t tmp;
            // Exclude the address and checksum
            tmp.assign(response.begin() + 1, response.begin() + RESPONSE_LENGTH - 1);
            protocol::marshaller m;
            const int16_t value = m.unmarshall(tmp);
            LOG4CXX_DEBUG(logger, "Received: " << value);
            return value;
        }
    }
    return boost::none;
}

boost::optional<int16_t> read_register(const int fd, const int16_t address, const int timeout)
{
    LOG4CXX_TRACE(logger, "Constructing the message");

    protocol::composer c;
    const protocol::byte_sequence_t message = c.create_message(0x02, address, 0x0000);

    LOG4CXX_DEBUG(logger, "Writing the message " << to_hex(message));

    const ssize_t bytes_written = write(fd, message.c_str(), message.length());

    LOG4CXX_DEBUG(logger, "Wrote " << bytes_written << " bytes");

    LOG4CXX_TRACE(logger, "Reading the response");

    protocol::byte_sequence_t response;
    const unsigned int response_length = read_response(fd, timeout, response);

    LOG4CXX_DEBUG(logger, "Raw data: " << to_hex(response));

    return handle_response(response, response_length);
}

void write_register(const int fd, const int16_t address, const int16_t value, const int timeout)
{
    LOG4CXX_TRACE(logger, "Constructing the message");

    protocol::composer c;
    const protocol::byte_sequence_t message = c.create_message(0x03, address, value);

    LOG4CXX_DEBUG(logger, "Writing the message: '" << to_hex(message) << "'");

    const ssize_t bytes_written = write(fd, message.c_str(), message.length());

    LOG4CXX_DEBUG(logger, "Wrote " << bytes_written << " bytes");

    LOG4CXX_TRACE(logger, "Reading the response");

    // @todo Does Rego send a response? Yes, but only one character. Which character in which situation?
    protocol::byte_sequence_t response;
    const unsigned int response_length = read_response(fd, timeout, response);

    LOG4CXX_DEBUG(logger, "Raw data: " << to_hex(response));

    handle_response(response, response_length);
}

} // namespace util
} // namespace rego
