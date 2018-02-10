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

#pragma once

#include <boost/program_options.hpp>
#include <string>

namespace rego {
namespace util {

/**
 * Print usage
 *
 * @param executable The name of the executable
 * @param options The command line option
 */
void print_usage(const std::string& executable, const boost::program_options::options_description& options);

} // namespace util
} // namespace rego
