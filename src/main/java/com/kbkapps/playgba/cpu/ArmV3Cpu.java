package com.kbkapps.playgba.cpu;

import com.kbkapps.playgba.cpu.constants.Flags;
import com.kbkapps.playgba.cpu.constants.Instructions;
import com.kbkapps.playgba.cpu.opcodes.armv3.ArithmeticLogical;
import com.kbkapps.playgba.cpu.opcodes.armv3.Branch;
import com.kbkapps.playgba.cpu.opcodes.armv3.OpCode;
import com.kbkapps.playgba.cpu.opcodes.armv3.SingleDataTransfer;

import static com.kbkapps.playgba.cpu.constants.Flags.CC;
import static com.kbkapps.playgba.cpu.constants.Flags.CS;

public class ArmV3Cpu {
    public static final Flags HS = CS;
    public static final Flags LO = CC;
    public static final int SP = 13;
    public static final int LR = 14;
    public static final int PC = 15;
    public static final int N = 0x80_00_00_00;
    public static final int Z = 0x40_00_00_00;
    public static final int C = 0x20_00_00_00;
    public static final int V = 0x10_00_00_00;
    Registers reg;
    GbaMemory gbaMemory;

    public ArmV3Cpu(Registers registers) {
        reg = registers;
    }

    public ArmV3Cpu(Registers registers, GbaMemory gbaMemory) {
        this(registers);
        this.gbaMemory = gbaMemory;
    }

    public void execute(OpCode opcode) throws UndefinedOpcodeException {
        if (opcode == null) {
            System.out.println("Nothing to execute");
            return;
        } else System.out.printf("Executing PC=0x%x: %s\n", reg.getPC() - 4, opcode);
        if (opcode.getInstruction() == Instructions.B | opcode.getInstruction() == Instructions.BL) {
            if (reg.canExecute(opcode.getCondition())) {
                branch(opcode);
            }
        } else if (opcode.getInstruction() == Instructions.BX) {
            if (reg.canExecute(opcode.getCondition()))
                exBranch(opcode);
        } else if (opcode.getInstruction() == Instructions.MRS | opcode.getInstruction() == Instructions.MSR) {
            if (reg.canExecute(opcode.getCondition()))
                psrTransfer(opcode);
        } else if (opcode.getInstruction() == Instructions.ADD) {
            if (reg.canExecute(opcode.getCondition()))
                add(opcode);
        } else if (opcode.getInstruction() == Instructions.TEQ) {
            if (reg.canExecute(opcode.getCondition()))
                testExclusive(opcode);
        } else if (opcode.getInstruction() == Instructions.CMP) {
            if (reg.canExecute(opcode.getCondition()))
                compare(opcode);
        } else if (opcode.getInstruction() == Instructions.ORR) {
            if (reg.canExecute(opcode.getCondition()))
                logicalOr(opcode);
        } else if (opcode.getInstruction() == Instructions.MOV) {
            if (reg.canExecute(opcode.getCondition()))
                move(opcode);
        } else if (opcode.getInstruction() == Instructions.LDR) {
            if (reg.canExecute(opcode.getCondition()))
                loadReg((SingleDataTransfer) opcode);
        } else if (opcode.getInstruction() == Instructions.STR) {
            if (reg.canExecute(opcode.getCondition()))
                storeReg((SingleDataTransfer) opcode);
        } else {
            throw new UndefinedOpcodeException(opcode.toString());
        }
    }

    private void exBranch(OpCode opcode) {
        if (opcode.getInstruction() == Instructions.BX) {
            int regNo = reg.getReg(opcode.getRegNo());
            boolean thumbMode = (regNo & 1) != 0;
            regNo = regNo & 0xFF_FF_FF_FE;
//            System.out.printf("regNo = 0x%x\n", regNo);
//            System.out.println("thumbMode = " + thumbMode);
            if (thumbMode)
                reg.setPSR(Registers.CPSR, reg.getPSR(Registers.CPSR) | 0x20);
            reg.setPC(regNo);
        }
    }

    private void add(OpCode opcode) {
        ArithmeticLogical alu = (ArithmeticLogical) opcode;
        if (alu.hasImmediate()) {
            int result = reg.getReg(alu.getRegNo()) + alu.getImmediate();
            System.out.printf("result = 0x%x\n", result);
            reg.setReg(alu.getRegDest(), result);
            if (alu.canChangePsr())
                reg.setPSR(Registers.CPSR, setFlags(result));
        }
    }

    private void branch(OpCode opcode) {
        Branch br = (Branch) opcode;
        if (br.getInstruction() == Instructions.BL){
            reg.setReg(LR, reg.getReg(PC));
//            System.out.printf("LR=0x%x\n", reg.getReg(LR));
        }
        reg.setPC((int) (Integer.toUnsignedLong(reg.getReg(PC)) + br.getOffset() * 4));
    }

    private void logicalOr(OpCode opcode) {
        ArithmeticLogical alu = (ArithmeticLogical) opcode;
        if (alu.hasImmediate()) {
            int result = reg.getReg(alu.getRegNo()) | alu.getImmediate();
//            System.out.println("result = " + Integer.toUnsignedString((int) result, 16));
            reg.setReg(alu.getRegDest(), result);
            if (alu.canChangePsr())
                reg.setPSR(Registers.CPSR, setFlags(result));
        }
    }

    private void psrTransfer(OpCode opcode) {
        ArithmeticLogical alu = (ArithmeticLogical) opcode;
        if (alu.getInstruction() == Instructions.MRS)
            reg.setReg(alu.getRegDest(), reg.getPSR(alu.getPsr()));
        else
            reg.setPSR(alu.getPsr(), alu.getMask() & reg.getReg(alu.getRegSource()));
    }

    private void testExclusive(OpCode opcode) {
        if (opcode.hasImmediate()) {
            int before = reg.getReg(opcode.getRegNo());
            int result = before ^ opcode.getImmediate();
//            System.out.println("result = " + Integer.toUnsignedString(result, 16));
            reg.setPSR(Registers.CPSR, setFlags(result));
        }
    }

    private void loadReg(SingleDataTransfer opcode) {
        if (opcode.hasImmediate()) {
            int address = 0;
            if (opcode.shouldAddOffsetBeforeTransfer()) {
                int regNValue = reg.getReg(opcode.getRegNo());
                if (opcode.shouldAdd())
                    address = regNValue + opcode.getImmediate();    //[r12+0x300]
                else
                    address = regNValue - opcode.getImmediate();    //[r12-0x300]
                int data;
                if (opcode.isByteTransfer())
                    data = gbaMemory.read8(address);
                else
                    data = gbaMemory.read32(address);
                int regDest = opcode.getRegDest();
                if (regDest == 15)
                    reg.setPC(data - 12);
                else
                    reg.setReg(regDest, data);
            } else {
                if (opcode.shouldAdd()) {
                    //[r12]+0x300
                } else {
                    //[r12]-0x300
                }
            }
        }
    }

    private void storeReg(SingleDataTransfer opcode) {
        if (opcode.hasImmediate()) {
            if (opcode.shouldAddOffsetBeforeTransfer()) {
                int regNo = reg.getReg(opcode.getRegNo());
                int address = 0;
                if (opcode.shouldAdd())
                    address = regNo + opcode.getImmediate();    //[r12+0x300]
                else
                    address = regNo - opcode.getImmediate();    //[r12-0x300]
//                System.out.printf("Address=0x%x\n", address);
                regNo = opcode.getRegDest();
                int regNValue = regNo == PC ? reg.getPC() : reg.getReg(regNo);
                try {
                    if (opcode.isByteTransfer())
                        gbaMemory.write8(address, (byte) regNValue);
                    else
                        gbaMemory.write32(address, regNValue);
                } catch (WriteDeniedException e) {
                    e.printStackTrace();
                    System.exit(-1);
                }
            } else {
                if (opcode.shouldAdd()) {
                    //[r12]+0x300
                } else {
                    //[r12]-0x300
                }
            }
        }
    }

    private void move(OpCode opcode) {
        ArithmeticLogical alu = (ArithmeticLogical) opcode;
        if (alu.hasImmediate()) {
            int result = alu.getImmediate();
//            System.out.println("result = " + Integer.toUnsignedString((int) result, 16));
            reg.setReg(alu.getRegDest(), result);
            if (alu.canChangePsr())
                reg.setPSR(Registers.CPSR, setFlags(result));
        }
    }

    private void compare(OpCode opcode) {
        if (opcode.hasImmediate()) {
            long before = reg.getReg(opcode.getRegNo());
            long immediate = opcode.getImmediate();
            long result = before - immediate;
//            System.out.println("result = " + Integer.toUnsignedString((int) result, 16));
            boolean isSub = true;
            reg.setPSR(Registers.CPSR, setFlags(result, isSub));
        }
    }

    /*
    Addition Overflow occurs if
        (+A) + (+B) = −C
        (−A) + (−B) = +C
    Subtraction Overflow occurs if
        (+A) − (−B) = −C
        (−A) − (+B) = +C
    */

    private int setFlags(long result, boolean expectsPositive) {
        int flags = setFlags(result);
        System.out.println("expectsPositive = " + expectsPositive);
//        if (expectsPositive) {
//        System.out.println("result = " + Long.toUnsignedString(result, 16));
//        System.out.println("(int)result = " + Integer.toUnsignedString((int) result, 16));
//          if ((result>>32)>0)
//              flags |= C;
//        if ((result >> 32) > 0)
//            flags |= V;
//        } else {
//            System.out.println("result = " + Long.toUnsignedString(result, 16));
//            System.out.println("(int)result = " + Integer.toUnsignedString((int) result, 16));
//          if ((result>>32)>0)
//              flags |= C;
//            if ((result >> 32) > 0)
//                flags |= V;
//        }
        return flags;
    }

    private int setFlags(long result) {
        int flags = 0;
        if (((result >> 31) & 1) != 0)
            flags |= N;
        if (((int) result) == 0)
            flags |= Z;
//        if ((result>>32)>0)
//            flags |= C;
// TODO       if ((result >> 32) > 0)
//            flags |= V;
        return flags;
    }
}
