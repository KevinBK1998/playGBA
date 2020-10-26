Feature: The Instruction Set

  Scenario Outline: Correct instruction is decoded
    When i try to decode <opcodes>
    Then i should see "<message>"
    Examples:
      | opcodes     | message                          |
      | 18 00 00 ea | always branch 0x18               |
      | 00 00 5e e3 | always compare lr 0x0            |
      | 04 e0 a0 03 | if equal move lr 0x4             |
      | 01 c3 a0 e3 | always move r12 0x4000000           |
      | 00 c3 dc e5 | always load byte r12, [r12 + 0x300] |
  #TODO:Add more instructions
  Scenario: Branch instruction is executed
  Bit    Explanation
  27-25  Must be "101" for this instruction
  24=0:   B{cond} label    ;branch            PC=PC+8+nn*4
  23-0   nn - Signed Offset, step 4      (-32M..+32M in steps of 4)
  Execution Time: 2S + 1N
  Return: No flags affected.
    Given pc is 8
    When i try to execute 18 00 00 ea
    Then pc must be 108
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
    When i try to execute 00 00 5e e3
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
    When i try to execute 04 e0 a0 03
    And R14 must be 4
    When i try to execute 01 c3 a0 e3
    And R12 must be 0x4000000

#  Scenario: Single Data Transfer (From Memory)
#  Bit    Expl.
#  31-28  Condition (Must be 1111b for PLD)
#  27-26  Must be 01b for this instruction
#  25     I - Immediate Offset Flag (0=Immediate, 1=Shifted Register)
#  24     P - Pre/Post (0=post; add offset after transfer, 1=pre; before trans.)
#  23     U - Up/Down Bit (0=down; subtract offset from base, 1=up; add to base)
#  22     B - Byte/Word bit (0=transfer 32bit/word, 1=transfer 8bit/byte)
#  ->When above Bit 24 P=0 (Post-indexing, write-back is ALWAYS enabled):
#  -->21     T - Memory Management (0=Normal, 1=Force non-privileged access)
#  ->When above Bit 24 P=1 (Pre-indexing, write-back is optional):
#  -->21     W - Write-back bit (0=no write-back, 1=write address into base)
#  20     L - Load/Store bit (0=Store to memory, 1=Load from memory)
#  ->0: STR{cond}{B}{T} Rd,<Address>   ;[Rn+/-<offset>]=Rd
#  ->1: LDR{cond}{B}{T} Rd,<Address>   ;Rd=[Rn+/-<offset>]
#  ->(1: PLD <Address> ;Prepare Cache for Load, see notes below)
#  ->Whereas, B=Byte, T=Force User Mode (only for POST-Indexing)
#  19-16  Rn - Base register               (R0..R15) (including R15=PC+8)
#  15-12  Rd - Source/Destination Register (R0..R15) (including R15=PC+12)
#  ->When above I=0 (Immediate as Offset)
#  -->11-0   Unsigned 12bit Immediate Offset (0-4095, steps of 1)
#  ->When above I=1 (Register shifted by Immediate as Offset)
#  -->11-7   Is - Shift amount      (1-31, 0=Special/See below)
#  -->6-5    Shift Type             (0=LSL, 1=LSR, 2=ASR, 3=ROR)
#  -->4      Must be 0 (Reserved, see The Undefined Instruction)
#  -->3-0    Rm - Offset Register   (R0..R14) (not including PC=R15)

  Scenario: Load Register with Immediate Byte
  Bit    Expl.
  31-28  Condition
  27-26  Must be 01b for this instruction
  25     I - Immediate Offset Flag (0=Immediate, 1=Shifted Register)
  24     P - Pre/Post (0=post; add offset after transfer, 1=pre; before trans.)
  23     U - Up/Down Bit (0=down; subtract offset from base, 1=up; add to base)
  22     B - Byte/Word bit (0=transfer 32bit/word, 1=transfer 8bit/byte)
  20     L - Load/Store bit (0=Store to memory, 1=Load from memory)
  ->1: LDR{cond}{B}{T} Rd,<Address>   ;Rd=[Rn+/-<offset>]
  ->Whereas, B=Byte, T=Force User Mode (only for POST-Indexing)
  19-16  Rn - Base register               (R0..R15) (including R15=PC+8)
  15-12  Rd - Source/Destination Register (R0..R15) (including R15=PC+12)
  ->When above I=0 (Immediate as Offset)
  -->11-0   Unsigned 12bit Immediate Offset (0-4095, steps of 1)
    When i try to execute 00 c3 dc e5
    Then R12 must be 0
