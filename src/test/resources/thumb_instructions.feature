Feature: The Thumb Instruction Set

  Scenario Outline: Correct instruction is decoded
    When I try to decode <opcodes>
    Then I should see "<message>"
    Examples:
      | opcodes | message                   |
      | 00 20   | move(s) r0, 0x0           |
      | 58 49   | load pc-relative r1, 0x58 |

#  THUMB.3: move/compare/add/subtract immediate
#  15-13  Must be 001b for this type of instructions
#  12-11  Opcode
#  00b: MOV{S} Rd,#nn      ;move     Rd   = #nn
#  01b: CMP{S} Rd,#nn      ;compare  Void = Rd - #nn
#  10b: ADD{S} Rd,#nn      ;add      Rd   = Rd + #nn
#  11b: SUB{S} Rd,#nn      ;subtract Rd   = Rd - #nn
#  10-8   Rd - Destination Register  (R0..R7)
#  7-0    nn - Unsigned Immediate    (0-255)
#  ARM equivalents for MOV/CMP/ADD/SUB are MOVS/CMP/ADDS/SUBS same format.
#  Execution Time: 1S
#  Return: Rd contains result (except CMP), N,Z,C,V affected (for MOV only N,Z).
  Scenario: Move immediate
    When I try to execute 00 20
    Then r0 must be 0
    And cpsr must be 40 00 00 00

#  THUMB.6: load PC-relative (for loading immediates from literal pool)
#  15-11  Must be 01001b for this type of instructions
#  N/A    Opcode (fixed)
#  LDR Rd,[PC,#nn]      ;load 32bit    Rd = WORD[PC+nn]
#  10-8   Rd - Destination Register   (R0..R7)
#  7-0    nn - Unsigned offset        (0-1020 in steps of 4)
#  The value of PC will be interpreted as (($+4) AND NOT 2).
#  Return: No flags affected, data loaded into Rd.
#  Execution Time: 1S+1N+1I

  Scenario: Load PC - relative immediate
    Given the pc is 288
    When I try to execute 58 49
    Then r1 must be 0xfffffe00