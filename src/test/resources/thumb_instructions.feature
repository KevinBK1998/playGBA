Feature: The Thumb Instruction Set

  Scenario Outline: Correct instruction is decoded
    When I try to decode <opcodes>
    Then I should see "<message>"
    Examples:
      | opcodes | message                   |
      | 00 20   | move-s r0, 0x0            |
      | 58 49   | load pc-relative r1, 0x58 |
      | 60 50   | store word r0, [r4+r1]    |
      | 09 1d   | add r1, r1, 0x4           |

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
    Given the pc is 290
    When I try to execute 58 49
    Then r1 must be 0xfffffe00

#  THUMB.7: load/store with register offset
#  15-12  Must be 0101b for this type of instructions
#  11-10  Opcode (0-3)
#  0: STR  Rd,[Rb,Ro]   ;store 32bit data  WORD[Rb+Ro] = Rd
#  1: STRB Rd,[Rb,Ro]   ;store  8bit data  BYTE[Rb+Ro] = Rd
#  2: LDR  Rd,[Rb,Ro]   ;load  32bit data  Rd = WORD[Rb+Ro]
#  3: LDRB Rd,[Rb,Ro]   ;load   8bit data  Rd = BYTE[Rb+Ro]
#  9      Must be zero (0) for this type of instructions
#  8-6    Ro - Offset Register              (R0..R7)
#  5-3    Rb - Base Register                (R0..R7)
#  2-0    Rd - Source/Destination Register  (R0..R7)
#  Return: No flags affected, data loaded either into Rd or into memory.
#  Execution Time: 1S+1N+1I for LDR, or 2N for STR
#
#  THUMB.8: load/store sign-extended byte/halfword
#  15-12  Must be 0101b for this type of instructions
#  11-10  Opcode (0-3)
#  0: STRH Rd,[Rb,Ro]  ;store 16bit data          HALFWORD[Rb+Ro] = Rd
#  1: LDSB Rd,[Rb,Ro]  ;load sign-extended 8bit   Rd = BYTE[Rb+Ro]
#  2: LDRH Rd,[Rb,Ro]  ;load zero-extended 16bit  Rd = HALFWORD[Rb+Ro]
#  3: LDSH Rd,[Rb,Ro]  ;load sign-extended 16bit  Rd = HALFWORD[Rb+Ro]
#  9      Must be set (1) for this type of instructions
#  8-6    Ro - Offset Register              (R0..R7)
#  5-3    Rb - Base Register                (R0..R7)
#  2-0    Rd - Source/Destination Register  (R0..R7)
#  Return: No flags affected, data loaded either into Rd or into memory.
#  Execution Time: 1S+1N+1I for LDR, or 2N for STR

  Scenario: Store word into address
    Given that r0 is 0
    And that r1 is 0xfffffe00
    And that r4 is 0x4000000
    When I try to execute 60 50
    Then 0 should be present in the memory 0x3fffe00

#  THUMB.2: add/subtract
#  15-11  Must be 00011b for 'add/subtract' instructions
#  10-9   Opcode (0-3)
#  0: ADD{S} Rd,Rs,Rn   ;add register        Rd=Rs+Rn
#  1: SUB{S} Rd,Rs,Rn   ;subtract register   Rd=Rs-Rn
#  2: ADD{S} Rd,Rs,#nn  ;add immediate       Rd=Rs+nn
#  3: SUB{S} Rd,Rs,#nn  ;subtract immediate  Rd=Rs-nn
#  Pseudo/alias opcode with Imm=0:
#  2: MOV{ADDS} Rd,Rs      ;move (affects cpsr) Rd=Rs+0
#  8-6    For Register Operand:
#  Rn - Register Operand (R0..R7)
#  For Immediate Operand:
#  nn - Immediate Value  (0-7)
#  5-3    Rs - Source register       (R0..R7)
#  2-0    Rd - Destination register  (R0..R7)
#  Return: Rd contains result, N,Z,C,V affected (including MOV).
#  Execution Time: 1S
  Scenario: Add regs
    Given that r1 is 0xfffffe00
    When I try to execute 09 1d
    Then r1 must be 0xfffffe04
    And cpsr must be 80 00 00 00
