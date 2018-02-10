/*
SPDX-License-Identifier: GPL-2.0-or-later

rego -- tools for interfacing with the Rego ground heat pump controller
Copyright (C) 2016,2018  Hannu Lounento

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
#include "util/cli.hpp"
#include "util/helpers.hpp"
#include "util/serial.hpp"

#include <boost/optional.hpp>
#include <boost/program_options.hpp>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <log4cxx/logger.h>
#include <log4cxx/helpers/properties.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>

namespace po = boost::program_options;

using namespace log4cxx;
using namespace log4cxx::helpers;

using namespace rego::protocol;
using namespace rego::util;

namespace {

/*
Initializes log4cxx to be used for logging
*/
void initialize_logging(const std::string& log_level)
{
    if (log_level != "info" &&
        log_level != "debug" &&
        log_level != "trace")
    {
        std::cerr << "Invalid log level: '" << log_level << std::endl;
        std::exit(EXIT_FAILURE);
    }

    try
    {
        LoggerPtr root = Logger::getRootLogger();
        log4cxx::helpers::Properties properties;
        const std::string rootLoggerConfiguration = log_level + ", stdout";
        properties.put("log4j.rootLogger", rootLoggerConfiguration);
        properties.put("log4j.appender.stdout", "org.apache.log4j.ConsoleAppender");
        properties.put("log4j.appender.stdout.layout", "org.apache.log4j.PatternLayout");
        properties.put("log4j.appender.stdout.layout.ConversionPattern", "%m\n");
        PropertyConfigurator::configure(properties);

    }
    catch(const Exception&)
    {
        std::cerr << "Failed to configure logging (log4cxx)" << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

// @todo Return int16_t to avoid the need for a cast
/*
Parses a Rego register address

@param address_str [in] The address to be parsed as a string

@return The address as an integer
*/
long int parse_address(const std::string& address_str)
{
    char* end = nullptr;
    const long int address = strtol(address_str.c_str(), &end, 0);
    if (address < 0 ||
        address > INT16_MAX)
    {
        throw std::invalid_argument("Address out of range: " + address_str);
    }
    else if (address == 0 && end == address_str.c_str())
    {
        throw std::invalid_argument("Malformed address: " + address_str);
    }
    return address;
}

// @todo Combine with parse_address
/*
Parses a value (to be) carried by the Rego protocol

@param value_str [in] The value to be parsed

@return The value as an integer
*/
long int parse_value(const std::string& value_str)
{
    char* end = nullptr;
    const long int value = strtol(value_str.c_str(), &end, 0);
    // @todo Are negative values needed / allowed?
    if (value < 0 ||
        value > INT16_MAX)
    {
        throw std::invalid_argument("Value out of range: " + value_str);
    }
    else if (value == 0 && end == value_str.c_str())
    {
        throw std::invalid_argument("Malformed value: " + value_str);
    }
    return value;
}

constexpr int INVALID_FD = -1;

// Whether verbose (debug) level messages should be printed (to stdout)
bool is_verbose = false;

LoggerPtr logger(Logger::getLogger("regoctl"));

} // anonymous namespace

int main(int argc, char** argv)
{
    std::vector<bool> verbose_flags;
    po::options_description options("Options");
    // @todo Handle read, scan, etc as subcommands
    options.add_options()
        ("scan,s", po::value<std::string>(), "scan a range of registers [0x0,0x7fff]")
        ("help,h", "produce help message")
        ("port", po::value<std::string>(), "the serial port for communications with Rego")
        ("read,r", po::value<std::string>(), "read from a register [0x0,0x7fff]")
        ("write,w", po::value<std::string>(), "write to a register [0x0,0x7fff]")
        ("timeout,t", po::value<int>(), "time-out for reading response (s)")
        ("value,e", po::value<std::string>(), "the value to be written [0x0,0x7fff])")
        ("log-level,l", po::value<std::string>()->default_value("info"),
            "log level: info, debug, trace");

    po::positional_options_description pod;
    pod.add("port", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(options).positional(pod).run(), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        rego::util::print_usage(argv[0], options);
        return 0;
    }

    const std::string log_level = vm["log-level"].as<std::string>();
    initialize_logging(log_level);

    if (vm.count("port") != 1)
    {
        LOG4CXX_ERROR(logger, "Port is a required argument");
        rego::util::print_usage(argv[0], options);
    }
    const std::string port_path = vm["port"].as<std::string>();
    static constexpr int DEFAULT_RESPONSE_TIMEOUT = 2;
    const int timeout = vm.count("timeout") ? vm["timeout"].as<int>() : DEFAULT_RESPONSE_TIMEOUT;
    if (is_verbose)
    {
        std::cout << "Opening the serial port " << port_path << std::endl;
    }

    struct termios oldtio;
    int fd = INVALID_FD;

    // Move read, write, scan into their own functions and / or files
    try {
        if (vm.count("read"))
        {
            // @todo Replace with an overloaded Rego_638::sensor_query(int16_t) method
            const std::string address_str = vm["read"].as<std::string>();
            const long int address = parse_address(address_str);
            fd = open_serial_port(port_path, oldtio);
            if (fd < 0)
            {
                return EXIT_FAILURE;
            }
            const boost::optional<int16_t> value =
                read_register(fd, static_cast<int16_t>(address), timeout);
            if (value)
            {
                std::cout << "Received: " << *value << std::endl;
            }
        }
        else if (vm.count("write"))
        {
            if (vm.count("value") != 1)
            {
                rego::util::print_usage(argv[0], options);
                return EXIT_FAILURE;
            }
            const std::string address_str = vm["write"].as<std::string>();
            const long int address = parse_address(address_str);
            const std::string value_str = vm["value"].as<std::string>();
            const long int value = parse_value(value_str);
            fd = open_serial_port(port_path, oldtio);
            if (fd < 0)
            {
                return EXIT_FAILURE;
            }
            write_register(fd, static_cast<int16_t>(address), static_cast<int16_t>(value), timeout);
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
                LOG4CXX_ERROR(logger, "Invalid range: " << range);
                rego::util::print_usage(argv[0], options);
                return EXIT_FAILURE;
            }
            const std::string range_start_str = range.substr(0, separator_pos);
            const std::string range_end_str = range.substr(separator_pos + 1);
            std::cout << range_start_str << std::endl;
            std::cout << range_end_str << std::endl;
            char* end = nullptr;
            const long int range_start = strtol(range_start_str.c_str(), &end, 0);
            if (range_start < 0 ||
                range_start > INT16_MAX - 1 ||
                (range_start == 0 && end == range_start_str.c_str()))
            {
                LOG4CXX_ERROR(logger, "Invalid range start: " << range_start_str);
                rego::util::print_usage(argv[0], options);
                return EXIT_FAILURE;
            }
            const long int range_end = strtol(range_end_str.c_str(), nullptr, 0);
            if (range_end < 1 ||
                range_end > INT16_MAX ||
                (range_end == 0 && end == range_end_str.c_str()))
            {
                LOG4CXX_ERROR(logger, "Invalid range end: " << range_end_str);
                rego::util::print_usage(argv[0], options);
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
                const boost::optional<int16_t> value =
                    read_register(fd, static_cast<int16_t>(address), timeout);
                if (value)
                {
                    std::cout << "Received: " << *value << std::endl;
                }
            }
        }
    }
    catch (std::exception& e)
    {
        LOG4CXX_ERROR(logger, e.what());
    }

    if (fd >= 0)
    {
        close_serial_port(fd, oldtio);
        fd = INVALID_FD;
    }

    if (is_verbose)
    {
        std::cout << "Done" << std::endl;
    }

    return EXIT_SUCCESS;
}
