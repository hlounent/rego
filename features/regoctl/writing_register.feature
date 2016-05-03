Feature: Writing to registers
  As an IVT Greenline HT Plus ground heat pump owner,
  I want to write values to Rego's registers
  in order to control the pump.

  Scenario Outline: Input validation
    When I run `regoctl --write <register> --value 0x1 /tmp/rego`
    Then the output should contain:
    """
    Address out of range
    """

  Examples:
    | register |
    |       -1 |
    |   0x8000 |
    |  0x10000 |


  Scenario Outline: Input validation
    When I run `regoctl --write <register> --value 0x1 /tmp/rego`
    Then the output should contain:
    """
    Malformed address
    """

  Examples:
    | register |
    |   string |


  Scenario Outline: Input validation
    When I run `regoctl --write 0x1 --value <value> /tmp/rego`
    Then the output should contain:
    """
    Value out of range
    """

  Examples:
    | value   |
    |      -1 |
    |  0x8000 |
    | 0x10000 |


  Scenario Outline: Input validation
    When I run `regoctl --write 0x1 --value <register> /tmp/rego`
    Then the output should contain:
    """
    Malformed value
    """

  Examples:
    | register |
    |   string |


  @fake-serial-device
  Scenario Outline: Writing a value to a register
    Given Rego is connected via serial line /tmp/rego
    When I successfully run `regoctl --write <register> --value <value> /tmp/rego`
    Then only the message <message> should be written to /tmp/rego

  Examples:
    | register | value | message            |
    |   0x0000 |    60 | 810300000000003c3c |
    |   0x0029 |    60 | 810300002900003c15 |
    |   0x7fff |    60 | 8103017f7f00003c3d |
