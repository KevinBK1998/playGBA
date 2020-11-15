Feature: The Instruction Set

  Scenario Outline: Correct instruction is decoded
    When i try to decode <opcodes>
    Then i should see "<message>"
    Examples:
      | opcodes     | message                               |
      | 18 00 00 ea | always branch 18                      |
      | 00 00 5e e3 | always compare lr, 0x0                |
      | 04 e0 a0 03 | if equal move lr, 0x4                 |
      | 01 c3 a0 e3 | always move r12, 0x4000000            |
      | 00 c3 dc e5 | always load byte r12, [r12 + 0x300]   |
      | 01 00 3c e3 | always exclusive test r12, 0x1        |
      | 00 c0 0f 01 | if equal move psr to reg r12, CPSR    |
      | c0 c0 8c 03 | if equal logical or r12, r12, 0xc0    |
      | 0c f0 29 01 | if equal move reg to psr CPSR_fc, r12 |
      | e3 ff ff 0a | if equal branch -1d                   |
      | df 00 a0 e3 | always move r0, 0xdf                  |
      | 00 f0 29 e1 | always move reg to psr CPSR_fc, r0    |
      | 01 43 a0 e3 | always move r4, 0x4000000             |
      | 08 42 c4 e5 | always store byte r4, [r4 + 0x208]    |
      | 0f 00 00 eb | always branch with link f             |
      | d3 00 a0 e3 | always move r0, 0xd3                  |
      | d0 d0 9f e5 | always load word sp, [pc + 0xd0]      |
      | 00 e0 a0 e3 | always move lr, 0x0                   |
      | 0e f0 69 e1 | always move reg to psr SPSR_fc, lr    |
      | d2 00 a0 e3 | always move r0, 0xd2                  |
      | b8 d0 9f e5 | always load word sp, [pc + 0xb8]      |
      | 5f 00 a0 e3 | always move r0, 0x5f                  |
      | a0 d0 9f e5 | always load word sp, [pc + 0xa0]      |
      | 01 00 8f e2 | always add r0, pc, 0x1                |
      | 10 ff 2f e1 | always bx r0                          |
  #TODO:Add more instructions

#  Scenario: Branch, Branch with Link (B, BL, BLX_imm)
#  Branch with Link is meant to be used to call to a subroutine, return
#  address is then saved in R14.
#  Bit    |Explanation
#  31-28  |Condition (must be 1111b for BLX)
#  27-25  |Must be "101" for this instruction
#  24     |Opcode (0-1) (or Half-word Offset for BLX)
#  ->0:   B{cond} label    ;branch            PC=PC+8+nn*4
#  ->1: BL{cond} label   ;branch/link
#  PC=PC+8+nn*4, LR=PC+4
#  ->H: BLX label ;ARM9  ;branch/link/thumb PC=PC+8+nn*4+H*2, LR=PC+4, T=1
#  23-0   |nn - Signed Offset, step 4      (-32M..+32M in steps of 4)
#  Branch with Link can be used to 'call' to a sub-routine, which may then 'return' by MOV PC,R14 for example.
#  Execution Time: 2S + 1N
#  Return: No flags affected.
#    Given All registers are initialised
#    And PC is <prevPC>
#    When I try to execute B <cond> <label>
#    Then I should be at <expectedPC>
  Scenario: Branch is executed
    Given pc is 8
    When i try to execute 18 00 00 ea
    Then pc must be 108
    Given pc is 140
    And CPSR is 00 00 00 00
    When i try to execute e3 ff ff 0a
    Then pc must be 144

  Scenario: Branch with Link is executed
    Given pc is 164
    When i try to execute 0f 00 00 eb
    Then pc must be 228
    And lr must be 168

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
#  Returned CPSR Flags
#If S=1, Rd<>R15, logical operations (AND,EOR,TST,TEQ,ORR,MOV,BIC,MVN):
#  V=not affected
#  C=carryflag of shift operation (not affected if LSL#0 or Rs=00h)
#  Z=zeroflag of result
#  N=signflag of result (result bit 31)
#If S=1, Rd<>R15, arithmetic operations (SUB,RSB,ADD,ADC,SBC,RSC,CMP,CMN):
#  V=overflowflag of result
#  C=carryflag of result
#  Z=zeroflag of result
#  N=signflag of result (result bit 31)
#IF S=1, with unused Rd bits=1111b, {P} opcodes (CMPP/CMNP/TSTP/TEQP):
#  R15=result  ;modify PSR bits in R15, ARMv2 and below only.
#  In user mode only N,Z,C,V bits of R15 can be changed.
#  In other modes additionally I,F,M1,M0 can be changed.
#  The PC bits in R15 are left unchanged in all modes.
#If S=1, Rd=R15; should not be used in user mode:
#  CPSR = SPSR_<current mode>
#  PC = result
#  For example: MOVS PC,R14  ;return from SWI (PC=R14_svc, CPSR=SPSR_svc).
#If S=0: Flags are not affected (not allowed for CMP,CMN,TEQ,TST).
#
#The instruction "MOV R0,R0" is used as "NOP" opcode in 32bit ARM state.
#Execution Time: (1+p)S+rI+pN. Whereas r=1 if I=0 and R=1 (ie. shift by register); otherwise r=0. And p=1 if Rd=R15; otherwise p=0.
#  9: TEQ{cond}{P}    Rn,Op2    ;test exclusive  Void = Rn XOR Op2
  Scenario: Add (with immediate) instruction is executed
    And pc is 284
    When i try to execute 01 00 8f e2
    Then R0 must be 0x11d

  Scenario: Test Exclusive (with immediate) instruction is executed
    When i try to execute 01 00 3c e3
    Then CPSR must be 00 00 00 00

  Scenario: Compare (with immediate) instruction is executed
    Given CPSR is 00 00 00 00
    When i try to execute 00 00 5e e3
    Then CPSR must be 40 00 00 00

  Scenario: Logical Or (with immediate) instruction is executed
    Given CPSR is 00 00 00 00
    And R12 is 0x0
    When i try to execute c0 c0 8c 03
    Then R12 must be 0x0
    And CPSR must be 00 00 00 00

  Scenario: Move (with immediate) instruction is executed
    Given CPSR is 40 00 00 00
    When i try to execute 04 e0 a0 03
    Then lr must be 4
    When i try to execute 01 c3 a0 e3
    Then R12 must be 0x4000000
    When i try to execute 5f 00 a0 e3
    Then R0 must be 0x5f
    When i try to execute d2 00 a0 e3
    Then R0 must be 0xd2
    When i try to execute d3 00 a0 e3
    Then R0 must be 0xd3
    When i try to execute df 00 a0 e3
    Then R0 must be 0xdf
    When i try to execute 00 e0 a0 e3
    Then lr must be 0

#  Scenario: PSR Transfer
#  These instructions occupy an unused area (TEQ,TST,CMP,CMN with S=0) of ALU opcodes.
#  Bit    Expl.
#  31-28  Condition
#  27-26  Must be 00b for this instruction
#  25     I - Immediate Operand Flag  (0=Register, 1=Immediate) (Zero for MRS)
#  24-23  Must be 10b for this instruction
#  22     Psr - Source/Destination PSR  (0=CPSR, 1=SPSR_<current mode>)
#  21     Opcode
#  0: MRS{cond} Rd,Psr          ;Rd = Psr
#  1: MSR{cond} Psr{_field},Op  ;Psr[field] = Op
#  20     Must be 0b for this instruction (otherwise TST,TEQ,CMP,CMN)
#  For MRS:
#  19-16   Must be 1111b for this instruction (otherwise SWP)
#  15-12   Rd - Destination Register  (R0-R14)
#  11-0    Not used, must be zero.
#  For MSR:
#  19      f  write to flags field     Bit 31-24 (aka _flg)
#  18      s  write to status field    Bit 23-16 (reserved, don't change)
#  17      x  write to extension field Bit 15-8  (reserved, don't change)
#  16      c  write to control field   Bit 7-0   (aka _ctl)
#  15-12   Not used, must be 1111b.
#  For MSR Psr,Rm (I=0)
#  11-4    Not used, must be zero. (otherwise BX)
#  3-0     Rm - Source Register <op>  (R0-R14)
#  For MSR Psr,Imm (I=1)
#  11-8    Shift applied to Imm   (ROR in steps of two 0-30)
#  7-0     Imm - Unsigned 8bit Immediate
#  In source code, a 32bit immediate should be specified as operand.
#  The assembler should then convert that into a shifted 8bit value.
  Scenario: Move to Reg from cpSr
    Given CPSR is 00 00 00 00
    When i try to execute 00 c0 0f 01
    Then R12 must be 0

  Scenario: Move to spSr (fc) from Reg
    Given CPSR is 00 00 00 d2
    And lr is 0
    When i try to execute 0e f0 69 e1
    Then SPSR must be 00 00 00 00
    Given CPSR is 00 00 00 d3
    And lr is 0
    When i try to execute 0e f0 69 e1
    Then SPSR must be 00 00 00 00

  Scenario: Move to cpSr (fc) from Reg
    Given R12 is 0x0
    When i try to execute 0c f0 29 01
    Then CPSR must be 00 00 00 00
    Given R0 is 0x5f
    When i try to execute 00 f0 29 e1
    Then CPSR must be 00 00 00 5f
    And Irq must be enabled
    And Fiq must be disabled
    And CPU must run in ARM
    And mode must be sys
    Given R0 is 0xd2
    When i try to execute 00 f0 29 e1
    Then CPSR must be 00 00 00 d2
    And Irq must be disabled
    And Fiq must be disabled
    And CPU must run in ARM
    And mode must be irq
    Given R0 is 0xd3
    When i try to execute 00 f0 29 e1
    Then CPSR must be 00 00 00 d3
    And Irq must be disabled
    And Fiq must be disabled
    And CPU must run in ARM
    And mode must be svc
    Given R0 is 0xdf
    When i try to execute 00 f0 29 e1
    Then CPSR must be 00 00 00 df
    And Irq must be disabled
    And Fiq must be disabled
    And CPU must run in ARM
    And mode must be sys


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
#  Return: CPSR flags are not affected.
#  Execution Time: For normal LDR: 1S+1N+1I. For LDR PC: 2S+2N+1I. For STR: 2N.
  Scenario: Load Register with byte from memory
    Given byte 0 is present in memory 0x4000300
    And R12 is 0x4000000
    When i try to execute 00 c3 dc e5
    Then R12 must be 0

  Scenario: Load Register with word from memory
    #Given word 0x03007f00 is present in memory 0x1b8
    And pc is 280
    When i try to execute a0 d0 9f e5
    Then SP must be 0x03007f00
    #Given word 0x03007fa0 is present in memory 0x1bc
    And pc is 260
    When i try to execute b8 d0 9f e5
    Then SP must be 0x03007fa0
    #Given word 0x03007fe0 is present in memory 0x1c0
    And pc is 240
    When i try to execute d0 d0 9f e5
    Then SP must be 0x03007fe0

  Scenario: Store Register to a byte in memory
    And R4 is 0x4000000
    When i try to execute 08 42 c4 e5
    Then 0 should be present in memory 0x4000208
