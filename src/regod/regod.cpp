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

#include "regod/sensors.hpp"
#include "util/cli.hpp"
#include "util/helpers.hpp"
#include "util/serial.hpp"

#include <boost/optional.hpp>
#include <boost/program_options.hpp>
#include <csignal>
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include <log4cxx/propertyconfigurator.h>

namespace po = boost::program_options;

using namespace log4cxx;
using namespace log4cxx::helpers;
using namespace rego::protocol;
using namespace rego::regod;
using namespace rego::util;

namespace {

constexpr int INVALID_FD = -1;

// Whether verbose (debug) level messages should be printed (to stdout)
bool is_verbose = false;

LoggerPtr logger(Logger::getLogger("regod"));

volatile std::sig_atomic_t quit = false;

void signal_handler(int /*signal*/)
{
    quit = true;
}

void set_signal_handler()
{
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGTERM, &sa, nullptr);
}

} // anonymous namespace

int main(int argc, char** argv)
{
    po::options_description options("Options");
    options.add_options()
        ("help,h", "produce help message")
        ("log-config", po::value<std::string>(), "path to the log configuration file")
        ("port", po::value<std::string>(), "the serial port for communications with Rego")
        ("system-bus", "connect to the system bus")
        ("session-bus", "connect to the session bus")
        ("timeout,t", po::value<int>(), "timeout (ms) for receiving a response from Rego")
        ("verbose,v", "print verbose output");

    po::positional_options_description pod;
    pod.add("port", 1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(options).positional(pod).run(), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        print_usage(argv[0], options);
        return 0;
    }

    if (vm.count("log-config"))
    {
        static const char* const DEFAULT_LOG_CONFIG_PATH = "/etc/regod/log.conf";
        log4cxx::PropertyConfigurator::configure(DEFAULT_LOG_CONFIG_PATH);
    }
    else
    {
        log4cxx::BasicConfigurator::configure();
    }

    is_verbose = vm.count("verbose");

    if (vm.count("port") == 0)
    {
        return EXIT_FAILURE;
    }
    const std::string port_path = vm["port"].as<std::string>();
    LOG4CXX_TRACE(logger, "Opening serial port " << port_path);
    struct termios oldtio;
    int fd = open_serial_port(port_path, oldtio);
    if (fd < 0)
    {
        return EXIT_FAILURE;
    }

    static constexpr auto path = "/fi/iki/halo/Rego1";
    LOG4CXX_TRACE(logger, "Connecting to bus");
    auto bus = vm.count("session-bus") ?
        sdbusplus::bus::new_user() : sdbusplus::bus::new_system();
    LOG4CXX_TRACE(logger, "Requesting name");
    bus.request_name("fi.iki.halo.Rego1");

    static constexpr int default_timeout = 2000;
    const int timeout = vm.count("timeout") ? vm["timeout"].as<int>() : default_timeout;
    LOG4CXX_TRACE(logger, "Creating service object");
    rego::regod::sensors sensors_object{bus, path, fd, timeout};

    set_signal_handler();

    while (!quit)
    {
        // Discard any unhandled messages
        bus.process_discard();
        bus.wait(timeout);
    }
    LOG4CXX_TRACE(logger, "Terminating");

    close_serial_port(fd, oldtio);

    return EXIT_SUCCESS;
}
