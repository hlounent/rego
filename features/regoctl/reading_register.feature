# SPDX-License-Identifier: GPL-2.0-or-later

Feature: Reading registers
  As an IVT Greenline HT Plus ground heat pump owner,
  I want to read values from Rego's registers
  in order to inspect the ground heat pump configuration.

  Background:
    Given Rego is connected via serial line /tmp/rego

  @fake-serial-device
  Scenario Outline: Reading a register specified on the command line
    Given Rego is connected via serial line /tmp/rego
    When I successfully run `regoctl --read <register> /tmp/rego`
    Then only the message <message> should be written to /tmp/rego

  Examples:
    | register | message            |
    |   0x020b | 810200040b0000000f |


  @fake-serial-device
  Scenario Outline: A valid response
    Given I run `regoctl --read 0x020b /tmp/rego` in background
    When Rego sends the response <response>
    Then the output should contain:
    """
    Received: <value>
    """

  Examples:
    | response   | value |
    | 010002383a |   312 |


  @fake-serial-device
  Scenario Outline: A valid response in multiple parts
    Given I run `regoctl --read 0x020b /tmp/rego` in background
    When Rego sends the response <part1>
    When there is a delay of <delay> ms
    When Rego sends the response <part2>
    Then the output should contain:
    """
    Received: <value>
    """

  Examples:
    | delay   | part1    | part2     | value |
    |     100 |       01 |  0002383a |   312 |
    |     100 |     0100 |    02383a |   312 |
    |     100 | 01000238 |        3a |   312 |


  @fake-serial-device
  Scenario Outline: An invalid response
    Given I run `regoctl --read 0x020b /tmp/rego` in background
    When Rego sends the response <response>
    Then the output should contain:
    """
    Invalid message: incorrect destination address
    """

  Examples:
    | response   |
    | 000002383a |
    | 020002383a |
    | 7f0002383a |


  @fake-serial-device
  Scenario Outline: Too short response
    Given I run `regoctl --read 0x020b /tmp/rego` in background
    When Rego sends the response <response>
    Then the output should contain:
    """
    Invalid message: too short message: <length>
    """

  Examples:
    | response   | length |
    |         11 |      1 |
    |   11111111 |      4 |


  @fake-serial-device
  Scenario Outline: Too long response
    Given I run `regoctl --read 0x020b /tmp/rego` in background
    When Rego sends the response <response>
    Then the output should contain:
    """
    Invalid message: too long message: <length>
    """

  Examples:
    | response               | length |
    |           010000000000 |      6 |
    |         01111111111111 |      7 |
    |       0111111111111111 |      8 |
    |     011111111111111111 |      9 |
    |   01111111111111111111 |     10 |
    | 0111111111111111111111 |     10 |


  @fake-serial-device
  Scenario Outline: Invalid checksum
    Given I run `regoctl --read 0x020b /tmp/rego` in background
    When Rego sends the response <response>
    Then the output should contain:
    """
    Invalid message: incorrect checksum
    """

  Examples:
    | response   |
    | 0100010300 |
    | 0100010301 |
    | 0100010303 |
    | 01000103fd |
    | 01000103ff |
