Feature: The CPU
  The Game Boy Advance Cpu is 32 bit ARM7TDMI and runs at 16MHz.

  Scenario: Pipelining
    When clock is triggered
    Then current instruction is executed
    And next instruction is decoded
    And the next instruction after that is fetched