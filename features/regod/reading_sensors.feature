# SPDX-License-Identifier: GPL-2.0-or-later

Feature: Reading all sensors
  As an IVT Greenline HT Plus ground heat pump owner,
  I want to read current values of Rego's sensors
  in order to log data about the heat pump operation

  @wip
  @fake-serial-device
  Scenario: Reading all sensors
    Given Rego is connected via serial line /tmp/rego
    Given I run `regod --timeout 10 --session-bus --verbose /tmp/rego` in background
    Given the D-Bus service fi.iki.halo.Rego1 exists
    When the D-Bus method fi.iki.halo.Rego1.Sensors.ReadAll on /fi/iki/halo/Rego1 is called
    Then a message should be written to /tmp/rego within 500 ms
    Then the message should contain the bytes 81020004090000000d
    Then the message should contain the bytes 810200040a0000000e
    Then the message should contain the bytes 810200040b0000000f
    Then the message should contain the bytes 810200040e0000000a
    Then the message should contain the bytes 810200040f0000000b
    Then the message should contain the bytes 810200041000000014
    Then the message should contain the bytes 810200041100000015
    Then the message should contain the bytes 810200041200000016
    Then the message should contain the bytes 810200006e0000006e
    Then the message should contain the bytes 810200006f0000006f
    Then the message should contain the bytes 810200007000000070
    Then the message should contain the bytes 810200002b0000002b
    Then the message should contain the bytes 810200007300000073
    Then the message should contain the bytes 810200007400000074
    Then the message should contain the bytes 810200037d0000007e
    Then the message should contain the bytes 810200037e0000007d
    Then the message should contain the bytes 810200037f0000007c
    Then the message should contain the bytes 810200040000000004
    Then the message should contain the bytes 810200040300000007
    Then the message should contain the bytes 810200040400000000
    Then the message should contain the bytes 810200040500000001
    Then the message should contain the bytes 810200040600000002
