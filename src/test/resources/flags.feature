Feature: The Instruction Set
  GBA has a 32 bit risc Cpu and every instruction is either 16 bit (thumb mode) or 32 bit.

  Background: Reset the CPU
    Given All registers are initialised

  Scenario Outline: Correct Flags are raised
  The flags available are:
  Bit   Expl.
  31    N - Sign Flag       (0=Not Signed, 1=Signed)               ;\
  30    Z - Zero Flag       (0=Not Zero, 1=Zero)                   ; Condition
  29    C - Carry Flag      (0=Borrow/No Carry, 1=Carry/No Borrow) ; Code Flags
  28    V - Overflow Flag   (0=No Overflow, 1=Overflow)            ;/
    Given CPSR is 0x00000000
    And r0 is 0x<reg>
    When I try to execute compare r0 0x<data>
    Then CPSR must be 0x<cpsr>
    Examples:
      | reg      | data | cpsr     |
      | 00000000 | 000  | 40000000 |
      | 00000001 | 000  | 00000000 |
      | 00000000 | 001  | 80000000 |

  Scenario Outline: Branch should only execute when condition is true
  Branch (B) is supposed to jump to a subroutine.
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
  F:   NV     -             never (ARMv1,v2 only) (Reserved ARMv3 and up)
    And PC is <prevPC>
    When I try to execute B <cond> <label>
    Then I should be at <expectedPC>
    Examples:
      | cond | prevPC | label | expectedPC |
      | EQ   | 4      | 2     | 4          |
      | NE   | 4      | 2     | 16         |
      | CS   | 4      | 2     | 4          |
      | CC   | 4      | 2     | 16         |
      | MI   | 4      | 2     | 4          |
      | PL   | 4      | 2     | 16         |
      | VS   | 4      | 2     | 4          |
      | VC   | 4      | 2     | 16         |
      | HI   | 4      | 2     | 4          |
      | LS   | 4      | 2     | 16         |
      | GE   | 4      | 2     | 16         |
      | LT   | 4      | 2     | 4          |
      | GT   | 4      | 2     | 16         |
      | LE   | 4      | 2     | 4          |
      | AL   | 4      | 2     | 16         |