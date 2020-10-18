Feature: The Instruction Set
  GBA has a 32 bit risc Cpu and every instruction is either 16 bit (thumb mode) or 32 bit.

  Scenario Outline: Branch
  Branch (B) is supposed to jump to a subroutine.

  Bit    Explanation
  31-28  Condition
  27-25  Must be "101" for this instruction
  24     Opcode (0-1)
  ->0:   B{cond} label    ;branch            PC=PC+8+nn*4
  23-0   nn - Signed Offset, step 4      (-32M..+32M in steps of 4)
  Execution Time: 2S + 1N
  Return: No flags affected.
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
#  Scenario: Branch with Link (B, BL, BLX_imm)
#  Branch with Link is meant to be used to call to a subroutine, return
#  address is then saved in R14.
#  Bit    |Explanation
#  31-28  |Condition (must be 1111b for BLX)
#  27-25  |Must be "101" for this instruction
#  24     |Opcode (0-1) (or Half-word Offset for BLX)
#  0: B{cond} label    ;branch            PC=PC+8+nn*4
#  1: BL{cond} label   ;branch/link       PC=PC+8+nn*4, LR=PC+4
#  H: BLX label ;ARM9  ;branch/link/thumb PC=PC+8+nn*4+H*2, LR=PC+4, T=1
#  23-0   |nn - Signed Offset, step 4      (-32M..+32M in steps of 4)
#  Branch with Link can be used to 'call' to a sub-routine, which may then 'return' by MOV PC,R14 for example.
#  Execution Time: 2S + 1N
#  Return: No flags affected.