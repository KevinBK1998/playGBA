Feature: The CPU
  The Game Boy Advance Cpu is 32 bit ARM7TDMI and runs at 16MHz.

  Scenario Outline: Correct instruction is decoded
    When i try to decode <opcodes>
    Then i should see "<message>"
    Examples:
      | opcodes     | message               |
      | 18 00 00 ea | branch always 0x18    |
      | 00 00 5e e3 | compare always lr 0x0 |
      | 04 e0 a0 03 | move equal lr 0x4     |
  #TODO:Add more instructions
  Scenario: Branch instruction is executed
  Bit    Explanation
  27-25  Must be "101" for this instruction
  24=0:   B{cond} label    ;branch            PC=PC+8+nn*4
  23-0   nn - Signed Offset, step 4      (-32M..+32M in steps of 4)
  Execution Time: 2S + 1N
  Return: No flags affected.
    Given pc is 0
    When i try to execute 18 00 00 ea
    Then pc must be 104
#  Scenario: Branch with Link ( BL, BLX_imm)
#  Branch with Link is meant to be used to call to a subroutine, return
#  address is then saved in R14.
#  Bit    |Explanation
#  31-28  |Condition (must be 1111b for BLX)
#  27-25  |Must be "101" for this instruction
#  24     |Opcode (0-1) (or Half-word Offset for BLX)
#  ->1: BL{cond} label   ;branch/link       PC=PC+8+nn*4, LR=PC+4
#  ->H: BLX label ;ARM9  ;branch/link/thumb PC=PC+8+nn*4+H*2, LR=PC+4, T=1
#  23-0   |nn - Signed Offset, step 4      (-32M..+32M in steps of 4)
#  Branch with Link can be used to 'call' to a sub-routine, which may then 'return' by MOV PC,R14 for example.
#  Execution Time: 2S + 1N
#  Return: No flags affected.
#    Given All registers are initialised
#    And PC is <prevPC>
#    When I try to execute B <cond> <label>
#    Then I should be at <expectedPC>

#  Scenario: ALU
#  Bit    Expl.
#  31-28  Condition
#  27-26  Must be 00b for this instruction
#  25     I - Immediate 2nd Operand Flag (0=Register, 1=Immediate)
#  24-21  Opcode (0-Fh)               ;*=Arithmetic, otherwise Logical
#  0: AND{cond}{S} Rd,Rn,Op2    ;AND logical       Rd = Rn AND Op2
#  1: EOR{cond}{S} Rd,Rn,Op2    ;XOR logical       Rd = Rn XOR Op2
#  2: SUB{cond}{S} Rd,Rn,Op2 ;* ;subtract          Rd = Rn-Op2
#  3: RSB{cond}{S} Rd,Rn,Op2 ;* ;subtract reversed Rd = Op2-Rn
#  4: ADD{cond}{S} Rd,Rn,Op2 ;* ;add               Rd = Rn+Op2
#  5: ADC{cond}{S} Rd,Rn,Op2 ;* ;add with carry    Rd = Rn+Op2+Cy
#  6: SBC{cond}{S} Rd,Rn,Op2 ;* ;sub with carry    Rd = Rn-Op2+Cy-1
#  7: RSC{cond}{S} Rd,Rn,Op2 ;* ;sub cy. reversed  Rd = Op2-Rn+Cy-1
#  8: TST{cond}{P}    Rn,Op2    ;test            Void = Rn AND Op2
#  9: TEQ{cond}{P}    Rn,Op2    ;test exclusive  Void = Rn XOR Op2
#  A: CMP{cond}{P}    Rn,Op2 ;* ;compare         Void = Rn-Op2
#  B: CMN{cond}{P}    Rn,Op2 ;* ;compare neg.    Void = Rn+Op2
#  C: ORR{cond}{S} Rd,Rn,Op2    ;OR logical        Rd = Rn OR Op2
#  D: MOV{cond}{S} Rd,Op2       ;move              Rd = Op2
#  E: BIC{cond}{S} Rd,Rn,Op2    ;bit clear         Rd = Rn AND NOT Op2
#  F: MVN{cond}{S} Rd,Op2       ;not               Rd = NOT Op2
#  20     S - Set Condition Codes (0=No, 1=Yes) (Must be 1 for opcode 8-B)
#  19-16  Rn - 1st Operand Register (R0..R15) (including PC=R15)
#  ->Must be 0000b for MOV/MVN.
#  15-12  Rd - Destination Register (R0..R15) (including PC=R15)
#  ->Must be 0000b (or 1111b) for CMP/CMN/TST/TEQ{P}.
#    When above Bit 25 I=0 (Register as 2nd Operand)
#    When below Bit 4 R=0 - Shift by Immediate
#  11-7   Is - Shift amount   (1-31, 0=Special/See below)
#    When below Bit 4 R=1 - Shift by Register
#  11-8   Rs - Shift register (R0-R14) - only lower 8bit 0-255 used
#  7      Reserved, must be zero  (otherwise multiply or LDREX or undefined)
#  6-5    Shift Type (0=LSL, 1=LSR, 2=ASR, 3=ROR)
#  4      R - Shift by Register Flag (0=Immediate, 1=Register)
#  3-0    Rm - 2nd Operand Register (R0..R15) (including PC=R15)
#    When above Bit 25 I=1 (Immediate as 2nd Operand)
#  11-8   Is - ROR-Shift applied to nn (0-30, in steps of 2)
#  7-0    nn - 2nd Operand Unsigned 8bit Immediate

  Scenario: Compare (with immediate) instruction is executed
  Bit    Expl.
  27-26  Must be 00b for this instruction
  25     I - Immediate 2nd Operand Flag (0=Register, 1=Immediate)
  24-21=A: CMP{cond}{P}    Rn,Op2 ;* ;compare         Void = Rn-Op2
  20     Must be 1 for opcode 8-B
  19-16  Rn - 1st Operand Register (R0..R15) (including PC=R15)
  15-12  Rd - Destination Register (R0..R15) (including PC=R15)
  ->Must be 0000b (or 1111b) for CMP/CMN/TST/TEQ{P}.
  when above Bit 25 I=1 (Immediate as 2nd Operand)
  ->11-8   Is - ROR-Shift applied to nn (0-30, in steps of 2)
  ->7-0    nn - 2nd Operand Unsigned 8bit Immediate
    Given pc is 104
    When i try to execute 00 00 5e e3
    Then pc must be 108
    And CPSR must be 40 00 00 00

  Scenario: Move (with immediate) instruction is executed
  Bit    Expl.
  27-26  Must be 00b for this instruction
  25     I - Immediate 2nd Operand Flag (0=Register, 1=Immediate)
  24-21=D: MOV{cond}{S} Rd,Op2       ;move              Rd = Op2
  20     S - Set Condition Codes (0=No, 1=Yes)
  19-16  Rn - 1st Operand Register (R0..R15) (including PC=R15)
  ->Must be 0000b for MOV/MVN.
  15-12  Rd - Destination Register (R0..R15) (including PC=R15)
  when above Bit 25 I=1 (Immediate as 2nd Operand)
  ->11-8   Is - ROR-Shift applied to nn (0-30, in steps of 2)
  ->7-0    nn - 2nd Operand Unsigned 8bit Immediate
    Given pc is 108
    And CPSR is 40 00 00 00
    When i try to execute 04 e0 a0 03
    Then pc must be 112
    And R14 must be 4
    And CPSR must be 40 00 00 00
