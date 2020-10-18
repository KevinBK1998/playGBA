Feature: The CPU
  The Game Boy Advance Cpu is ARM7TDMI and runs at 16MHz. It is a 32 bit risc Cpu and every instruction is either 16
  bit (thumb mode) or 32 bit.

  Code Suffix Flags         Meaning
  0:   EQ     Z=1           equal (zero) (same)
  1:   NE     Z=0           not equal (nonzero) (not same)
  2:   CS/HS  C=1           unsigned higher or same (carry set)
  3:   CC/LO  C=0           unsigned lower (carry cleared)
  4:   MI     N=1           signed negative (minus)
  5:   PL     N=0           signed positive or zero (plus)
  6:   VS     V=1           signed overflow (V set)
  7:   VC     V=0           signed no overflow (V cleared)
  8:   HI     C=1 and Z=0   unsigned higher
  9:   LS     C=0 or Z=1    unsigned lower or same
  A:   GE     N=V           signed greater or equal
  B:   LT     N<>V          signed less than
  C:   GT     Z=0 and N=V   signed greater than
  D:   LE     Z=1 or N<>V   signed less or equal
  E:   AL     -             always (the "AL" suffix can be omitted)
  #TODO:Add more functions
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
    And CPSR is <state>
    And PC is <prevPC>
    When I try to execute B <cond> <label>
    Then I should be at <expectedPC>
    Examples:
      | state | cond  | prevPC | label | expectedPC |
      | low   | EQ    | 0      | 2     | 0          |
      | low   | NE    | 0      | 2     | 8          |
      | low   | CS/HS | 0      | 2     | 0          |
      | low   | CC/LO | 0      | 2     | 8          |
      | low   | MI    | 0      | 2     | 0          |
      | low   | PL    | 0      | 2     | 8          |
      | low   | VS    | 0      | 2     | 0          |
      | low   | VC    | 0      | 2     | 8          |
      | low   | HI    | 0      | 2     | 0          |
      | low   | LS    | 0      | 2     | 8          |
      | low   | GE    | 0      | 2     | 8          |
      | low   | LT    | 0      | 2     | 0          |
      | low   | GT    | 0      | 2     | 8          |
      | low   | LE    | 0      | 2     | 0          |
      | low   | AL    | 0      | 2     | 8          |
      | low   | EQ    | 10     | 4     | 10         |
      | low   | NE    | 10     | 4     | 26         |
      | low   | CS/HS | 10     | 4     | 10         |
      | low   | CC/LO | 10     | 4     | 26         |
      | low   | MI    | 10     | 4     | 10         |
      | low   | PL    | 10     | 4     | 26         |
      | low   | VS    | 10     | 4     | 10         |
      | low   | VC    | 10     | 4     | 26         |
      | low   | HI    | 10     | 4     | 10         |
      | low   | LS    | 10     | 4     | 26         |
      | low   | GE    | 10     | 4     | 26         |
      | low   | LT    | 10     | 4     | 10         |
      | low   | GT    | 10     | 4     | 26         |
      | low   | LE    | 10     | 4     | 10         |
      | low   | AL    | 10     | 4     | 26         |
      | high  | EQ    | 0      | 2     | 8          |
      | high  | NE    | 0      | 2     | 0          |
      | high  | CS/HS | 0      | 2     | 8          |
      | high  | CC/LO | 0      | 2     | 0          |
      | high  | MI    | 0      | 2     | 8          |
      | high  | PL    | 0      | 2     | 0          |
      | high  | VS    | 0      | 2     | 8          |
      | high  | VC    | 0      | 2     | 0          |
      | high  | HI    | 0      | 2     | 0          |
      | high  | LS    | 0      | 2     | 8          |
      | high  | GE    | 0      | 2     | 8          |
      | high  | LT    | 0      | 2     | 0          |
      | high  | GT    | 0      | 2     | 0          |
      | high  | LE    | 0      | 2     | 8          |
      | high  | AL    | 0      | 2     | 8          |
      | high  | EQ    | 10     | 4     | 26         |
      | high  | NE    | 10     | 4     | 10         |
      | high  | CS/HS | 10     | 4     | 26         |
      | high  | CC/LO | 10     | 4     | 10         |
      | high  | MI    | 10     | 4     | 26         |
      | high  | PL    | 10     | 4     | 10         |
      | high  | VS    | 10     | 4     | 26         |
      | high  | VC    | 10     | 4     | 10         |
      | high  | HI    | 10     | 4     | 10         |
      | high  | LS    | 10     | 4     | 26         |
      | high  | GE    | 10     | 4     | 26         |
      | high  | LT    | 10     | 4     | 10         |
      | high  | GT    | 10     | 4     | 10         |
      | high  | LE    | 10     | 4     | 26         |
      | high  | AL    | 10     | 4     | 26         |
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
