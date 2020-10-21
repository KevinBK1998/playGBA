Feature: The Instruction Set
  GBA has a 32 bit risc Cpu and every instruction is either 16 bit (thumb mode) or 32 bit.

  Scenario Outline: Branch should only execute when condition is true
  Branch (B) is supposed to jump to a subroutine.
    Given All registers are initialised
    And PC is <prevPC>
    When I try to execute B <cond> <label>
    Then I should be at <expectedPC>
    Examples:
      | cond | prevPC | label | expectedPC |
      | EQ   | 0      | 2     | 0          |
      | NE   | 0      | 2     | 8          |
      | CS   | 0      | 2     | 0          |
      | CC   | 0      | 2     | 8          |
      | MI   | 0      | 2     | 0          |
      | PL   | 0      | 2     | 8          |
      | VS   | 0      | 2     | 0          |
      | VC   | 0      | 2     | 8          |
      | HI   | 0      | 2     | 0          |
      | LS   | 0      | 2     | 8          |
      | GE   | 0      | 2     | 8          |
      | LT   | 0      | 2     | 0          |
      | GT   | 0      | 2     | 8          |
      | LE   | 0      | 2     | 0          |
      | AL   | 0      | 2     | 8          |