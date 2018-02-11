# SPDX-License-Identifier: GPL-2.0-or-later

Feature: Scanning registers
  As a developer,
  I want to scan a range of addresses
  in order to check which registers exist,
  or to detect which register a changed setting is stored in.

  @fake-serial-device
  Scenario Outline: Scan a range of addresses
    Given Rego is connected via serial line /tmp/rego
    When I successfully run `regoctl --timeout 100 --scan <range> /tmp/rego`
    Then the message <message1> should be written to /tmp/rego
    Then only the message <message2> should be written to /tmp/rego

  Examples:
    | range         | message1           | message2           |
    | 0x0000-0x0001 | 810200000000000000 | 810200000100000001 |
    | 0x0aab-0x0aac | 810200152b0000003e | 810200152c00000039 |
    | 0x7ffe-0x7fff | 8102017f7e00000000 | 8102017f7f00000001 |
