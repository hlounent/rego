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

#include "protocol/byte_sequence_t.hpp"
#include "protocol/composer.hpp"

#include <boost/scoped_ptr.hpp>
#include <gtest/gtest.h>

using namespace rego::protocol;

namespace {

class ComposerTest : public ::testing::Test
{
public:
    ComposerTest()
      : _composer(new composer())
    {
    }

    ~ComposerTest()
    {
    }

    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    }

    boost::scoped_ptr<composer> _composer;
};

} // unnamed namespace

TEST_F(ComposerTest, checksumIsVerifiedCorrectly)
{
    byte_sequence_t m;
    m.append(1, 0x01);
    m.append(1, 0x02);

    composer c;
    const unsigned char checksum = c.calculate_checksum(m);
    m.append(1, checksum);

    ASSERT_TRUE(c.is_checksum_valid(m));
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
