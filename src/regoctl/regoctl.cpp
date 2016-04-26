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

// POSIX compliant source
#define _POSIX_SOURCE 1

#include "protocol/composer.hpp"
#include "protocol/marshaller.hpp"

#include <algorithm>
#include <boost/program_options.hpp>
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <poll.h>
#include <termios.h>
#include <vector>

namespace po = boost::program_options;
using namespace rego::protocol;

namespace {

// Constants
enum
{
    // The response packet is always 5 bytes long
    RESPONSE_LENGTH = 5,
    // The maximum time to wait for a response
    RESPONSE_TIMEOUT = 2
};

// Whether verbose (debug) level messages should be printed (to stdout)
bool is_verbose = false;

// Prints the given byte sequence to stdout in hexadecimal format
void print_in_hex(const byte_sequence_t& data)
{
    for (auto b : data)
    {
        printf(" 0x%.2x", static_cast<int>(b));
    }
}

// Print usage
void print_usage(const std::string& executable, const po::options_description& options)
{
    std::cout << "Usage: " << executable << " [options]\n";
    std::cout << options;
}

/*
Opens the given device node as a serial port

@param path [in] the path of the device (e.g. /dev/ttyUSB0)
@param oldtio [out] storage for saving port's current configuration
*/
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

// Closes the serial port associated with the given fd restoring it to the given configuration
void close_serial_port(const int fd, const struct termios& oldtio)
{
    tcsetattr(fd, TCSANOW, &oldtio);
    close(fd);
}

/*
Reads a response from Rego

@param fd [in] the file descriptor to read from
@param timeout [in] timeout (in seconds) for read
@param response [out] the buffer to write the received data to

Note: Due to poll, which does not decrement the timeout value like select does,
      the total timeout may be longer than the given value

@todo Measure time spent in poll and decrement the timeout value for subsequent calls
*/
unsigned int read_response(const int fd, const int timeout, byte_sequence_t& response)
{
    enum
    {
        // For receiving any extra bytes
        EXTRA_CAPACITY = 5
    };

    unsigned int bytes_received = 0;

    enum { N_FDS = 1 };
    struct pollfd fds[N_FDS];
    fds[0].fd = fd;
    fds[0].events = POLLIN;
    fds[0].revents = 0;

    while (bytes_received < RESPONSE_LENGTH)
    {
        const int rv = poll(fds, N_FDS, timeout * 1000);
        if (rv == -1)
        {
            std::cerr << "Failed to read response" << strerror(errno) << std::endl;
            break;
        }
        else if (rv == 0)
        {
            std::cerr << "Time-out!" << std::endl;
            break;
        }
        else
        {
            enum
            {
                BUFFER_LENGTH = 10
            };
            unsigned char read_buffer[BUFFER_LENGTH];
            const ssize_t received = read(fd, read_buffer, BUFFER_LENGTH);
            read_buffer[received] = 0;

            if (is_verbose)
            {
                std::cout << "Read " << received << " bytes" << std::endl;
            }
            response.append(read_buffer, static_cast<byte_sequence_t::size_type>(received));
            bytes_received += received;
        }
    }
    return bytes_received;
}

/*
Handles a response received from Rego

@param response [in] the response
@param length [in] the length of the response, i.e. how many bytes 'response' contains
*/
void handle_response(const byte_sequence_t& response, const unsigned int length)
{
    if (length == 0)
    {
        std::cerr << "No response received" << std::endl;
    }
    else if (length > 0 && length < RESPONSE_LENGTH)
    {
        std::cerr << "Invalid message: too short message: " << length << std::endl;
    }
    else if (length > RESPONSE_LENGTH)
    {
        std::cerr << "Invalid message: too long message: " << length << std::endl;
    }
    else if (length == RESPONSE_LENGTH)
    {
        composer c;
        if (response.at(0) != 0x01)
        {
            std::cerr << "Invalid message: incorrect destination address" << std::endl;
        }
        else if (!c.is_checksum_valid(byte_sequence_t(response.begin() + 1, response.begin() + RESPONSE_LENGTH)))
        {
            std::cerr << "Invalid message: incorrect checksum" << std::endl;
        }
        else
        {
            byte_sequence_t tmp;
            // Exclude the address and checksum
            tmp.assign(response.begin() + 1, response.begin() + RESPONSE_LENGTH - 1);
            marshaller m;
            const int16_t value = m.unmarshall(tmp);
            std::cout << "Received: " << value << std::endl;
        }
    }
}

/*
Reads a single register and handles the response

@param fd [in] the file descriptor to read from
@param address [in] the address of the register
@param timeout [in] timeout (in seconds) for read
*/
void read_register(const int fd, const int16_t address, const int timeout)
{
    if (is_verbose)
    {
        std::cout << "Constructing the message" << std::endl;
    }
    rego::protocol::composer c;
    const byte_sequence_t message = c.create_message(0x02, address, 0x0000);
    if (is_verbose)
    {
        std::cout << "Writing the message";
        print_in_hex(message);
        std::cout << std::endl;
    }
    const ssize_t bytes_written = write(fd, message.c_str(), message.length());
    if (is_verbose)
    {
        std::cout << "Wrote " << bytes_written << " bytes" << std::endl;
    }

    if (is_verbose)
    {
        std::cout << "Reading the response" << std::endl;
    }
    byte_sequence_t response;
    const unsigned int response_length = read_response(fd, timeout, response);
    if (is_verbose)
    {
        std::cout << "Raw data:";
        print_in_hex(response);
        std::cout << std::endl;
    }

    handle_response(response, response_length);
}

} // unnamed namespace

int main(int argc, char** argv)
{
    po::options_description options("Options");
    // @todo Handle read, scan, etc as subcommands
    options.add_options()
        ("scan,s", po::value<std::string>(), "scan a range of registers ]0x0,0x10000]")
        ("help,h", "produce help message")
        ("port", po::value<std::string>(), "the serial port for communications with Rego")
        ("read,r", po::value<std::string>(), "read from a register ]0x0,0x10000]")
        ("timeout,t", po::value<int>(), "time-out for reading response (s)")
        ("verbose,v", "print verbose output");

    po::positional_options_description pod;
    pod.add("port", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(options).positional(pod).run(), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        print_usage(argv[0], options);
        return 0;
    }

    is_verbose = vm.count("verbose");

    const std::string port_path = vm["port"].as<std::string>();
    const int timeout = vm.count("timeout") ? vm["timeout"].as<int>() : RESPONSE_TIMEOUT;
    if (is_verbose)
    {
        std::cout << "Opening the serial port " << port_path << std::endl;
    }

    struct termios oldtio;
    int fd = -1;

    // @todo Use log4cxx or similar for logging
    if (vm.count("read"))
    {
        // @todo Replace with an overloaded Rego_638::sensor_query(int16_t) method
        const std::string address_str = vm["read"].as<std::string>();
        char* end = NULL;
        const long int address = strtol(address_str.c_str(), &end, 0);
        // @todo Handle the error return value 0 when no conversion can be performed
        if (address < 0 ||
            address > INT16_MAX ||
            (address == 0 && end == address_str.c_str()))
        {
            std::cerr << "Invalid address: " << address_str << std::endl;
            print_usage(argv[0], options);
            return EXIT_FAILURE;
        }
        fd = open_serial_port(port_path, oldtio);
        if (fd < 0)
        {
            return EXIT_FAILURE;
        }
        read_register(fd, static_cast<int16_t>(address), timeout);
    }
    else if (vm.count("scan"))
    {
        const std::string range = vm["scan"].as<std::string>();
        const std::string::size_type separator_pos = range.find_first_of("-");
        if (separator_pos == std::string::npos ||
            // The possible index of the separator is 3: "0x0-"
            separator_pos < 3 ||
            // The minimum length is 7 characters: "0x0-0x1"
            range.length() < 7)
        {
            std::cerr << "Invalid range: " << range << std::endl;
            print_usage(argv[0], options);
            return EXIT_FAILURE;
        }
        const std::string range_start_str = range.substr(0, separator_pos);
        const std::string range_end_str = range.substr(separator_pos + 1);
        std::cout << range_start_str << std::endl;
        std::cout << range_end_str << std::endl;
        char* end = NULL;
        const long int range_start = strtol(range_start_str.c_str(), &end, 0);
        if (range_start < 0 ||
            range_start > INT16_MAX - 1 ||
            (range_start == 0 && end == range_start_str.c_str()))
        {
            std::cerr << "Invalid range start: " << range_start_str << std::endl;
            print_usage(argv[0], options);
            return EXIT_FAILURE;
        }
        const long int range_end = strtol(range_end_str.c_str(), NULL, 0);
        if (range_end < 1 ||
            range_end > INT16_MAX ||
            (range_end == 0 && end == range_end_str.c_str()))
        {
            std::cerr << "Invalid range end: " << range_end_str << std::endl;
            print_usage(argv[0], options);
            return EXIT_FAILURE;
        }
        fd = open_serial_port(port_path, oldtio);
        if (fd < 0)
        {
            return EXIT_FAILURE;
        }
        for (int address = static_cast<int16_t>(range_start); address <= static_cast<int16_t>(range_end); ++address)
        {
            printf("Reading register 0x%.2x\n", address);
            read_register(fd, static_cast<int16_t>(address), timeout);
        }
    }

    close_serial_port(fd, oldtio);

    if (is_verbose)
    {
        std::cout << "Done" << std::endl;
    }

    return EXIT_SUCCESS;
}
