# SPDX-License-Identifier: GPL-2.0-or-later

Feature: Help
  As a user,
  I want to have help printed out when invoking the application with the command line switch --help
  in order to be able to use the application effectively.
  
  Scenario: Application invoked with --help
    Given I successfully run `regoctl --help`
    Then the output should contain:
    """
    Usage:
    """ 
    And the output should contain:
    """
    Options:
    """