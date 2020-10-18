Feature: The CPU
  The Game Boy Advance Cpu is 32 bit ARM7TDMI and runs at 16MHz.

  Scenario: Correct instruction is decoded
    When i try to execute ea 00 00 18
    Then i should see "branch always 0x18"

  Scenario: Correct instruction is executed
    When i try to execute ea 00 00 18
    Then pc must be 96