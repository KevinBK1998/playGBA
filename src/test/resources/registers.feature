Feature: Registers
  There's a total of 37 registers (32bit each), 31 general registers (Rxx) and 6 status registers (xPSR). Note that only
  some registers are 'banked', for example, each mode has it's own R14 register: called R14, R14_fiq, R14_svc, etc. for
  each mode respectively. However, other registers are not banked, for example, each mode is using the same R0 register,
  so writing to R0 will always affect the content of R0 in other modes also. We will be ignoring reg_fiq as they are not
  used in GBA.

  Background: Reset the CPU
    Given All registers are initialised to zero

  Scenario Outline: There are 14 unbanked registers (R0-R12 and R15)
  R0-R12 Registers (General Purpose Registers): These thirteen registers may be used for whatever general purposes.
  Basically, each is having same functionality and performance, ie. there is no 'fast accumulator' for arithmetic
  operations, and no 'special pointer register' for memory addressing. However, in THUMB mode only R0-R7 (Lo registers)
  may be accessed freely, while R8-R12 and up (Hi registers) can be accessed only by some instructions.

  R15 Register (PC): R15 is always used as program counter (PC). Note that when reading R15, this will usually return a
  value of PC+nn because of read-ahead (pipelining), whereas 'nn' depends on the instruction and on the CPU state (ARM
  or THUMB).
    When I read from R<index>
    Then I should get 0
    When I write <data> to R<index>
    Then <data> should be present in R<index>
    When I switch mode to <mode>
    And I read from R<index>
    Then I should get <data>
    When I write 0 to R<index>
    Then 0 should be present in R<index>
    Examples:
      | mode | index | data |
      | svc  | 0     | 12   |
      | svc  | 1     | 45   |
      | svc  | 2     | 56   |
      | svc  | 3     | 15   |
      | svc  | 4     | 85   |
      | svc  | 5     | 43   |
      | svc  | 6     | 97   |
      | svc  | 7     | 23   |
      | svc  | 8     | 31   |
      | svc  | 9     | 225  |
      | svc  | 10    | 34   |
      | svc  | 11    | 234  |
      | svc  | 12    | 456  |
      | svc  | 15    | 56   |
      | abt  | 0     | 12   |
      | abt  | 1     | 45   |
      | abt  | 2     | 56   |
      | abt  | 3     | 15   |
      | abt  | 4     | 85   |
      | abt  | 5     | 43   |
      | abt  | 6     | 97   |
      | abt  | 7     | 23   |
      | abt  | 8     | 31   |
      | abt  | 9     | 225  |
      | abt  | 10    | 34   |
      | abt  | 11    | 234  |
      | abt  | 12    | 456  |
      | abt  | 15    | 56   |
      | irq  | 0     | 12   |
      | irq  | 1     | 45   |
      | irq  | 2     | 56   |
      | irq  | 3     | 15   |
      | irq  | 4     | 85   |
      | irq  | 5     | 43   |
      | irq  | 6     | 97   |
      | irq  | 7     | 23   |
      | irq  | 8     | 31   |
      | irq  | 9     | 225  |
      | irq  | 10    | 34   |
      | irq  | 11    | 234  |
      | irq  | 12    | 456  |
      | irq  | 15    | 56   |
      | und  | 0     | 12   |
      | und  | 1     | 45   |
      | und  | 2     | 56   |
      | und  | 3     | 15   |
      | und  | 4     | 85   |
      | und  | 5     | 43   |
      | und  | 6     | 97   |
      | und  | 7     | 23   |
      | und  | 8     | 31   |
      | und  | 9     | 225  |
      | und  | 10    | 34   |
      | und  | 11    | 234  |
      | und  | 12    | 456  |
      | und  | 15    | 56   |

  Scenario Outline: There are 2 banked registers
  R13 Register (SP): This register is used as Stack Pointer (SP) in THUMB state. While in ARM state the user may decided
  to use R13 and/or other register(s) as stack pointer(s), or as general purpose register. As shown in the table above,
  there's a separate R13 register in each mode, and (when used as SP) each exception handler may (and MUST!) use its own
  stack.

  R14 Register (LR): This register is used as Link Register (LR). That is, when calling to a sub-routine by a Branch
  with Link (BL) instruction, then the return address (ie. old value of PC) is saved in this register. Storing the
  return address in the LR register is obviously faster than pushing it into memory, however, as there's only one LR
  register for each mode, the user must manually push its content before issuing 'nested' subroutines. Same happens
  when an exception is called, PC is saved in LR of new mode. Note: In ARM mode, R14 may be used as general purpose
  register also, provided that above usage as LR register isn't required.
    When I read from R<index>
    Then I should get 0
    When I write <data> to R<index>
    Then <data> should be present in R<index>
    When I switch mode to <mode>
    And I read from R<index>
    Then I should get 0
    When I write <data> to R<index>
    Then <data> should be present in R<index>
    Examples:
      | mode | index | data |
      | svc  | 13    | 678  |
      | svc  | 14    | 910  |
      | abt  | 13    | 123  |
      | abt  | 14    | 345  |
      | irq  | 13    | 678  |
      | irq  | 14    | 910  |
      | und  | 13    | 678  |
      | und  | 14    | 910  |

  Scenario Outline: There is only one Current Program Status Register.
  CPSR: The current condition codes (flags) and CPU control bits are stored in the CPSR register.
    When I read from CPSR
    Then I should get 0
    When I write 295 to CPSR
    Then 295 should be present in CPSR
    When I switch mode to <mode>
    And I read from CPSR
    Then I should get 295
    When I write 308 to CPSR
    Then 308 should be present in CPSR
    Examples:
      | mode |
      | svc  |
      | abt  |
      | irq  |
      | und  |

  Scenario Outline: The Saved Program Status Register is banked
  SPSR (Program Status Registers) (ARMv3 and up): When an exception arises, the old CPSR is saved in the SPSR of the
  respective exception-mode (much like PC is saved in LR).
    When I read from SPSR
    Then I should get 0
    When I write <data> to SPSR
    Then <data> should be present in SPSR
    When I switch mode to <mode>
    And I read from SPSR
    Then I should get 0
    When I write <data> to SPSR
    Then <data> should be present in SPSR
    Examples:
      | mode | data |
      | svc  | 123  |
      | abt  | 456  |
      | irq  | 678  |
      | und  | 910  |