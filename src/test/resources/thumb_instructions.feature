Feature: The Thumb Instruction Set

  Scenario Outline: Correct instruction is decoded
    When I try to decode <opcodes>
    Then I should see "<message>"
    Examples:
      | opcodes | message                     |
      | 00 20   | move-s r0, 0x0              |
      | 58 49   | load pc-relative r1, 0x58   |
      | 60 50   | store word r0, [r4+r1]      |
      | 09 1d   | add r1, r1, 0x4             |
      | fc db   | branch signed less than, -4 |
      | 70 47   | exchanging branch lr        |
      | f0 b5   | push sp!, {r4,r5,r6,r7,lr}  |
      | 8d b0   | add sp, -0xd                |

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
    Then 0x0 should be present in the memory 0x3fffe00

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
  Scenario: Addition
    Given that r1 is 0xfffffe00
    When I try to execute 09 1d
    Then r1 must be 0xfffffe04
    And cpsr must be 80 00 00 00

#  THUMB.16: conditional branch
#  15-12  Must be 1101b for this type of instructions
#  11-8   Opcode/Condition (0-Fh)
#  0: BEQ label        ;Z=1         ;equal (zero) (same)
#  1: BNE label        ;Z=0         ;not equal (nonzero) (not same)
#  2: BCS/BHS label    ;C=1         ;unsigned higher or same (carry set)
#  3: BCC/BLO label    ;C=0         ;unsigned lower (carry cleared)
#  4: BMI label        ;N=1         ;signed  negative (minus)
#  5: BPL label        ;N=0         ;signed  positive or zero (plus)
#  6: BVS label        ;V=1         ;signed  overflow (V set)
#  7: BVC label        ;V=0         ;signed  no overflow (V cleared)
#  8: BHI label        ;C=1 and Z=0 ;unsigned higher
#  9: BLS label        ;C=0 or Z=1  ;unsigned lower or same
#  A: BGE label        ;N=V         ;signed greater or equal
#  B: BLT label        ;N<>V        ;signed less than
#  C: BGT label        ;Z=0 and N=V ;signed greater than
#  D: BLE label        ;Z=1 or N<>V ;signed less or equal
#  E: Undefined, should not be used
#  F: Reserved for SWI instruction (see SWI opcode)
#  7-0    Signed Offset, step 2 ($+4-256..$+4+254)
#  Destination address must by halfword aligned (ie. bit 0 cleared)
#  Return: No flags affected, PC adjusted if condition true
#  Execution Time:
#  2S+1N   if condition true (jump executed)
#  1S      if condition false

  Scenario: Conditional Branching
    Given the pc is 296
    And cpsr is 80 00 00 00
    When I try to execute fc db
    Then the pc must be 294

#  THUMB.5: Hi register operations/branch exchange
#  15-10  Must be 010001b for this type of instructions
#  9-8    Opcode (0-3)
#  0: ADD Rd,Rs   ;add        Rd = Rd+Rs
#  1: CMP Rd,Rs   ;compare  Void = Rd-Rs  ;CPSR affected
#  2: MOV Rd,Rs   ;move       Rd = Rs
#  2: NOP         ;nop        R8 = R8
#  3: BX  Rs      ;jump       PC = Rs     ;may switch THUMB/ARM
#  3: BLX Rs      ;call       PC = Rs     ;may switch THUMB/ARM (ARM9)
#  7      MSBd - Destination Register most significant bit (or BL/BLX flag)
#  6      MSBs - Source Register most significant bit
#  5-3    Rs - Source Register        (together with MSBs: R0..R15)
#  2-0    Rd - Destination Register   (together with MSBd: R0..R15)
#  Restrictions: For ADD/CMP/MOV, MSBs and/or MSBd must be set, ie. it is not allowed that both are cleared.
#    When using R15 (PC) as operand, the value will be the address of the instruction plus 4 (ie. $+4). Except for BX R15: CPU switches to ARM state, and PC is auto-aligned as (($+4) AND NOT 2).
#  For BX, MSBs may be 0 or 1, MSBd must be zero, Rd is not used/zero.
#  For BLX, MSBs may be 0 or 1, MSBd must be set, Rd is not used/zero.
#  For BX/BLX, when Bit 0 of the value in Rs is zero:
#  Processor will be switched into ARM mode!
#  If so, Bit 1 of Rs must be cleared (32bit word aligned).
#  Thus, BX PC (switch to ARM) may be issued from word-aligned address
#  only, the destination is PC+4 (ie. the following halfword is skipped).
#  BLX may not use R15. BLX saves the return address as LR=PC+3 (with thumb bit).
#  Using BLX R14 is possible (sets PC=Old_LR, and New_LR=retadr).
#  Assemblers/Disassemblers should use MOV R8,R8 as NOP (in THUMB mode).
#  Return: Only CMP affects CPSR condition flags!
#  Execution Time:
#  1S     for ADD/MOV/CMP
#  2S+1N  for ADD/MOV with Rd=R15, and for BX
  Scenario: Exchanging Branch is executed
    Given cpsr is 40 00 00 7f
    And the pc is 296
    And that r14 is 0xa8
    When I try to execute 70 47
    Then the pc must be 168
    And cpsr must be 40 00 00 5f
    And irq must be enabled
    And cpu must run in ARM
    And the mode must be sys

#  THUMB.14: push/pop registers
#  15-12  Must be 1011b for this type of instructions
#  11     Opcode (0-1)
#  0: PUSH {Rlist}{LR}   ;store in memory, decrements SP (R13)
#  1: POP  {Rlist}{PC}   ;load from memory, increments SP (R13)
#  10-9   Must be 10b for this type of instructions
#  8      PC/LR Bit (0-1)
#  0: No
#  1: PUSH LR (R14), or POP PC (R15)
#  7-0    Rlist - List of Registers (R7..R0)
#  In THUMB mode stack is always meant to be 'full descending', ie. PUSH is equivalent to 'STMFD/STMDB' and POP to 'LDMFD/LDMIA' in ARM mode.
#    Examples:
#    PUSH {R0-R3}     ;push R0,R1,R2,R3
#    PUSH {R0,R2,LR}  ;push R0,R2,LR
#    POP  {R4,R7}     ;pop R4,R7
#    POP  {R2-R4,PC}  ;pop R2,R3,R4,PC
#    Note: When calling to a sub-routine, the return address is stored in LR register, when calling further sub-routines, PUSH {LR} must be used to save higher return address on stack. If so, POP {PC} can be later used to return from the sub-routine.
#    POP {PC} ignores the least significant bit of the return address (processor remains in thumb state even if bit0 was cleared), when intending to return with optional mode switch, use a POP/BX combination (eg. POP {R3} / BX R3).
#    ARM9: POP {PC} copies the LSB to thumb bit (switches to ARM if bit0=0).
#    Return: No flags affected, SP adjusted, registers loaded/stored.
#    Execution Time: nS+1N+1I (POP), (n+1)S+2N+1I (POP PC), or (n-1)S+2N (PUSH).
  Scenario: Store reg list into memory
    Given that r4 is 0x4000000
    And that r5 is 0
    And that r6 is 0
    And that r7 is 0
    And that r13 is 0x3007f00
    And that r14 is 0xb4
    When I try to execute f0 b5
    Then 0xb4 should be present in the memory 0x3007efc
    Then 0x0 should be present in the memory 0x3007ef8
    Then 0x0 should be present in the memory 0x3007ef4
    Then 0x0 should be present in the memory 0x3007ef0
    Then 0x4000000 should be present in the memory 0x3007eec
    Then r13 must be 0x3007eec

#  THUMB.13: add offset to stack pointer
#  15-8   Must be 10110000b for this type of instructions
#  7      Opcode/Sign
#  0: ADD  SP,#nn       ;SP = SP + nn
#  1: ADD  SP,#-nn      ;SP = SP - nn
#  6-0    nn - Unsigned Offset    (0-508, step 4)
#  Return: No flags affected, SP adjusted.
#  Execution Time: 1S
  Scenario: Add offset to sp
    Given that r13 is 0x3007eec
    When I try to execute 8d b0
    Then r13 must be 0x3007eb8
