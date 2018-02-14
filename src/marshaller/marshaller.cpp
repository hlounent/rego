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

#include "protocol/byte_sequence_t.hpp"

#include <boost/program_options.hpp>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <vector>

namespace po = boost::program_options;

using rego::protocol::byte_sequence_t;
using rego::protocol::marshaller;

int main(int argc, char** argv)
{
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("hex,h", "print data in hex format (default)")
        ("dec,d", "print data in decimal format")
        ("data", po::value< std::vector<std::string> >(),
            "16 bit integers to be marshalled")
        ;

    po::positional_options_description pod;
    pod.add("data", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
              options(desc).positional(pod).run(), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << "Usage: options_description [options]\n";
        std::cout << desc;
        return 0;
    }

    marshaller m;
    const std::vector<std::string> data = vm["data"].as< std::vector<std::string> >();
    for (auto i = data.begin(); i != data.end(); ++i)
    {
        const std::string input = *i;
        char* end = NULL;
        const long value = std::strtol(input.c_str(), &end, 0);
        if (value == 0 && end == input.c_str())
        {
            std::cout << "Failed to parse '" << input << "'" << std::endl;
        }
        else if (INT16_MIN <= value && value <= INT16_MAX)
        {
            const byte_sequence_t bytes = m.marshall(static_cast<int16_t>(value));
            std::cout << value << ": ";
            for (auto j = bytes.begin(); j != bytes.end(); ++j)
            {
                printf(vm.count("dec") ? "%d " : "0x%x ", *j);
            }
            std::cout << std::endl;
        }
        else
        {
            std::cout << "Value '" << input << "' out of range" << std::endl;
        }
    }

    return 0;
}
