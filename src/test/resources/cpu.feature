Feature: The CPU
  The Game Boy Advance Cpu is ARM7TDMI and runs at 16MHz. It is a 32 bit risc Cpu and every instruction is either 16
  bit (thumb mode) or 32 bit. We will be ignoring reg_fiq as they are not used in GBA
  #TODO: add tests for cpu operations
#  The following table shows the ARM7TDMI register set which is available in each mode. There's a total of 37 registers (32bit each), 31 general registers (Rxx) and 6 status registers (xPSR).
#  Note that only some registers are 'banked', for example, each mode has it's own R14 register: called R14, R14_fiq, R14_svc, etc. for each mode respectively.
#  However, other registers are not banked, for example, each mode is using the same R0 register, so writing to R0 will always affect the content of R0 in other modes also.
#
#  System/User FIQ       Supervisor Abort     IRQ       Undefined
#  --------------------------------------------------------------
#  R13 (SP)    R13_fiq   R13_svc    R13_abt   R13_irq   R13_und
#  R14 (LR)    R14_fiq   R14_svc    R14_abt   R14_irq   R14_und
#  --------------------------------------------------------------
#  CPSR        CPSR      CPSR       CPSR      CPSR      CPSR
#  --          SPSR_fiq  SPSR_svc   SPSR_abt  SPSR_irq  SPSR_und
#  --------------------------------------------------------------

  Scenario Outline: There are 14 unbanked registers (R0-R12 and R15)
    Given All registers are initialised to zero
    When I read from R<index>
    Then I should get 0
    When I write <data> to R<index>
    Then <data> should be present in R<index>
    Examples:
      | index | data |
      | 0     | 12   |
      | 1     | 45   |
      | 2     | 56   |
      | 3     | 15   |
      | 4     | 85   |
      | 5     | 43   |
      | 6     | 97   |
      | 7     | 23   |
      | 8     | 31   |
      | 9     | 225  |
      | 10    | 34   |
      | 11    | 234  |
      | 12    | 456  |
      | 15    | 56   |

  Scenario Outline: There are 3 banked registers
    Given All registers are initialised to zero
    When I read from R<index>
    Then I should get 0
    When I write <data> to R<index>
    Then <data> should be present in R<index>
    Examples:
      | index | data |
      | 13    | 987  |
      | 14    | 314  |