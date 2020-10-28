package com.kbkapps.playgba.cpu;

import com.kbkapps.playgba.cpu.constants.Flags;
import com.kbkapps.playgba.cpu.constants.Instructions;
import com.kbkapps.playgba.cpu.opcodes.OpCode;
import com.kbkapps.playgba.cpu.opcodes.SingleDataTransfer;

import static com.kbkapps.playgba.cpu.constants.Flags.CC;
import static com.kbkapps.playgba.cpu.constants.Flags.CS;

public class ArmV3Cpu {
    public static final Flags HS = CS;
    public static final Flags LO = CC;
    public static final int PC = 15;
    public static final int Z = 0x40_00_00_00;
    Registers reg;
    GbaMemory gbaMemory;
    public static final int N = 0x80_00_00_00;

    public ArmV3Cpu(Registers registers) {
        reg = registers;
    }

    public ArmV3Cpu(Registers registers, GbaMemory gbaMemory) {
        this(registers);
        this.gbaMemory = gbaMemory;
    }

    public void runOpcode(String opCode, String condition, int label) {
        Flags cond = Flags.valueOf(condition);
        if (opCode.equals("B"))
            if (reg.canExecute(cond))
                branch(label);
    }

    private void branch(int label) {
        reg.setReg(PC, (int) (Integer.toUnsignedLong(getPC()) + label * 4));
    }

    public void execute(OpCode opcode) {
        if (opcode == null) {
            System.out.println("NOP");
            return;
        } else System.out.println("Executing: " + opcode);
        if (opcode.getInstruction() == Instructions.B) {
            if (reg.canExecute(opcode.getCondition())) {
                branch(opcode.getOffset());
            }
        } else if (opcode.getInstruction() == Instructions.CMP) {
            if (reg.canExecute(opcode.getCondition()))
                compare(opcode);
        } else if (opcode.getInstruction() == Instructions.MOV) {
            if (reg.canExecute(opcode.getCondition()))
                move(opcode);
        } else if (opcode.getInstruction() == Instructions.LDR) {
            if (reg.canExecute(opcode.getCondition()))
                loadReg((SingleDataTransfer) opcode);
        }
    }

    private void loadReg(SingleDataTransfer opcode) {
        if (opcode.hasImmediate()) {
            if (opcode.shouldAddOffsetBeforeTransfer()) {
                if (opcode.shouldAdd()) {
                    //[r12+0x300]
                    int regNo = reg.getReg(opcode.getRegNo());
                    int data = gbaMemory.read8(regNo + opcode.getImmediate());
                    regNo = opcode.getRegDest();
                    reg.setReg(regNo, data);
                } else {
                    //[r12-0x300]
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
        if (opcode.hasImmediate()) {
            int flags = 0;
            int result = opcode.getImmediate();
//            System.out.println("result = " + Integer.toUnsignedString((int) result, 16));
            if (((result >> 31) & 1) != 0)
                flags |= N;
            if (result == 0)
                flags |= Z;
            reg.setReg(opcode.getRegDest(), result);
            if (opcode.canChangePsr())
                reg.setPSR(Registers.CPSR, flags);
        }
    }

    private void compare(OpCode opcode) {
        if (opcode.hasImmediate()) {
            int flags = 0;
            long before = reg.getReg(opcode.getRegNo());
            long result = before - (long) opcode.getImmediate();
//            System.out.println("result = " + Integer.toUnsignedString((int) result, 16));
            if (((result >> 31) & 1) != 0)
                flags |= N;
            if (((int) result) == 0)
                flags |= Z;
            reg.setPSR(Registers.CPSR, flags);
            getPC();
        }
    }

    int getPC() {
        return reg.getReg(PC);
    }

    public void step() {
        reg.step();
    }

    public Registers getState() {
        return reg;
    }
}
