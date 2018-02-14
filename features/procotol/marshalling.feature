# SPDX-License-Identifier: GPL-2.0-or-later

Feature: Data marshalling
  In order to transmit data to the Rego device over serial line data needs to
  be marshalled into 7 bit long chunks most significant byte first.

  Scenario Outline: Marshalling a 16 bit signed integer
    When the value <input> is marshalled
    Then the result shall be <byte1> <byte2> <byte3>

  Examples:
    |  input | byte1 | byte2 | byte3 |
    | -32768 |     2 |     0 |     0 |
    |   -483 |     3 |   124 |    29 |
    |     -1 |     3 |   127 |   127 |
    |      0 |     0 |     0 |     0 |
    |    312 |     0 |     2 |    56 |
    |    523 |     0 |     4 |    11 |
    |  32767 |     1 |   127 |   127 |


  Scenario Outline: Unmarshalling a 16 bit integer
    When the byte stream <byte1> <byte2> <byte3> is unmarshalled
    Then the result shall be <output>

  Examples:
    | byte1 | byte2 | byte3 | output |
    |     2 |     0 |     0 | -32768 |
    |     3 |   124 |    29 |   -483 |
    |     3 |   127 |   127 |     -1 |
    |     0 |     0 |     0 |      0 |
    |     0 |     2 |    56 |    312 |
    |     0 |     4 |    11 |    523 |
    |     1 |   127 |   127 |  32767 |
