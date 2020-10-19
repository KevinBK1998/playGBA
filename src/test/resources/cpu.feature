Feature: The CPU
  The Game Boy Advance Cpu is 32 bit ARM7TDMI and runs at 16MHz.

  Scenario Outline: Correct instruction is decoded
    Given pc is <pc>
    When i try to execute <opcodes>
    Then i should see "<message>"
    Examples:
      | pc  | opcodes     | message            |
      | 0   | ea 00 00 18 | branch always 0x18 |
      | 100 | e2 5e f0 04 | cmp always lr  0x0 |

  Scenario: Branch instruction is executed
    Given pc is 0
    When i try to execute ea 00 00 18
    Then pc must be 100