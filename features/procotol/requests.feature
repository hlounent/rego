# SPDX-License-Identifier: GPL-2.0-or-later

Feature: Sending messages
  In order to request data from Rego a message consisting of 9 bytes needs to
  be sent to it.

  Scenario: Reading Rego version
    When the Rego version is requested to be read
    Then the message should be 129 127 0 0 0 0 0 0 0


  Scenario Outline: Reading sensor values
    When the sensor <sensor> is requested to be read
    Then the message should be 129 <byte2> <byte3> <byte4> <byte5> <byte6> <byte7> <byte8> <byte9>

  Examples:
    | sensor | byte2 | byte3 | byte4 | byte5 | byte6 | byte7 | byte8 | byte9 |
    |    GT1 |     2 |     0 |     4 |    11 |     0 |     0 |     0 |    15 |
    |    GT2 |     2 |     0 |     4 |    12 |     0 |     0 |     0 |     8 |
    |    GT3 |     2 |     0 |     4 |    13 |     0 |     0 |     0 |     9 |
    |    GT4 |     2 |     0 |     4 |    14 |     0 |     0 |     0 |    10 |
    |    GT5 |     2 |     0 |     4 |    15 |     0 |     0 |     0 |    11 |
    |    GT6 |     2 |     0 |     4 |    16 |     0 |     0 |     0 |    20 |
    |    GT8 |     2 |     0 |     4 |    17 |     0 |     0 |     0 |    21 |
    |    GT9 |     2 |     0 |     4 |    18 |     0 |     0 |     0 |    22 |
    |   GT10 |     2 |     0 |     4 |    19 |     0 |     0 |     0 |    23 |
    |   GT11 |     2 |     0 |     4 |    20 |     0 |     0 |     0 |    16 |
    |   GT3x |     2 |     0 |     4 |    21 |     0 |     0 |     0 |    17 |
