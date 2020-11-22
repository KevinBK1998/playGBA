Feature: The Thumb Instruction Set

  Scenario Outline: Correct instruction is decoded
    When I try to decode <opcodes>
    Then I should see "<message>"
    Examples:
      | opcodes | message      |
      | 00 20   | move r0, 0x0 |

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