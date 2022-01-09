Feature: The CPU
  The Game Boy Advance Cpu is 32 bit ARM7TDMI and runs at 16MHz.

  Scenario: Pipelining
    Given CPU runs 1 times
    When clock is triggered
    Then current instruction is executed
    And next instruction is fetched
    And next instruction is decoded
