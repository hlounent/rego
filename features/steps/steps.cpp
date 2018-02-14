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

#include "protocol/byte_sequence_t.hpp"
#include "protocol/marshaller.hpp"
#include "protocol/rego_638.hpp"

#include <boost/scoped_ptr.hpp>
// gtest.h needs to be included before cucumber-cpp/defs.hpp, otherwise
// cucumber will fail to report failures
#include <gtest/gtest.h>
#include <cucumber-cpp/defs.hpp>

#include <cstdint>
#include <cstdio>

using namespace rego::protocol;

struct marshaller_ctx
{
    marshaller_ctx()
      : _marshaller(new marshaller())
      , _marshalled()
      , _unmarshalled(0)
    {
    }

    ~marshaller_ctx()
    {
    }

    boost::scoped_ptr<marshaller> _marshaller;
    byte_sequence_t _marshalled;
    int16_t _unmarshalled;
};

using cucumber::ScenarioScope;

WHEN("^the value ([-]?[0-9]+) is marshalled$")
{
    REGEX_PARAM(int16_t, input);
    ScenarioScope<marshaller_ctx> context;

    context->_marshalled = context->_marshaller->marshall(input);
}

THEN("^the result shall be ([0-9]+) ([0-9]+) ([0-9]+)$")
{
    REGEX_PARAM(int, byte1);
    REGEX_PARAM(int, byte2);
    REGEX_PARAM(int, byte3);
    ScenarioScope<marshaller_ctx> context;

    const byte_sequence_t& result = context->_marshalled;
    EXPECT_EQ(result.size(), 3);
    EXPECT_EQ(byte1, result.at(0));
    EXPECT_EQ(byte2, result.at(1));
    EXPECT_EQ(byte3, result.at(2));
}

WHEN("^the byte stream ([0-9]+) ([0-9]+) ([0-9]+) is unmarshalled$")
{
    REGEX_PARAM(int, byte1);
    REGEX_PARAM(int, byte2);
    REGEX_PARAM(int, byte3);
    ScenarioScope<marshaller_ctx> context;

    context->_marshalled.push_back(static_cast<uint8_t>(byte1));
    context->_marshalled.push_back(static_cast<uint8_t>(byte2));
    context->_marshalled.push_back(static_cast<uint8_t>(byte3));

    context->_unmarshalled = context->_marshaller->unmarshall(context->_marshalled);
}

THEN("^the result shall be ([-]?[0-9]+)$")
{
    REGEX_PARAM(int, result);
    ScenarioScope<marshaller_ctx> context;

    EXPECT_EQ(result, context->_unmarshalled);
}


struct message_factory_ctx
{
    message_factory_ctx()
      : _rego_638(new rego_638())
      , _message()
    {
    }

    ~message_factory_ctx()
    {
    }

    boost::scoped_ptr<rego_638> _rego_638;
    byte_sequence_t _message;
};

WHEN("^the Rego version is requested to be read$")
{
    ScenarioScope<message_factory_ctx> context;

    context->_message.assign(context->_rego_638->version_query());
}

THEN("^the message should be ([0-9]+) ([0-9]+) ([0-9]+) ([0-9]+) ([0-9]+) ([0-9]+) ([0-9]+) ([0-9]+) ([0-9]+)$")
{
    REGEX_PARAM(int, byte1);
    REGEX_PARAM(int, byte2);
    REGEX_PARAM(int, byte3);
    REGEX_PARAM(int, byte4);
    REGEX_PARAM(int, byte5);
    REGEX_PARAM(int, byte6);
    REGEX_PARAM(int, byte7);
    REGEX_PARAM(int, byte8);
    REGEX_PARAM(int, byte9);
    ScenarioScope<message_factory_ctx> context;

    EXPECT_EQ(9, context->_message.length());
    EXPECT_EQ(byte1, context->_message.at(0));
    EXPECT_EQ(byte2, context->_message.at(1));
    EXPECT_EQ(byte3, context->_message.at(2));
    EXPECT_EQ(byte4, context->_message.at(3));
    EXPECT_EQ(byte5, context->_message.at(4));
    EXPECT_EQ(byte6, context->_message.at(5));
    EXPECT_EQ(byte7, context->_message.at(6));
    EXPECT_EQ(byte8, context->_message.at(7));
    EXPECT_EQ(byte9, context->_message.at(8));
}

WHEN("^the sensor ([a-zA-Z0-9]+) is requested to be read$")
{
    REGEX_PARAM(std::string, sensor);
    ScenarioScope<message_factory_ctx> context;

    context->_message.assign(context->_rego_638->sensor_query(sensor));
}
