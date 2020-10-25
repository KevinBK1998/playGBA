package com.kbkapps.playgba.cpu;

import com.kbkapps.playgba.cpu.constants.Flags;
import com.kbkapps.playgba.cpu.constants.Instructions;

import static com.kbkapps.playgba.cpu.constants.Flags.CC;
import static com.kbkapps.playgba.cpu.constants.Flags.CS;

public class ArmV3Cpu {
    public static final Flags HS = CS;
    public static final Flags LO = CC;
    public static final int PC = 15;
    public static final int Z = 0x40_00_00_00;
    Registers reg;
    public static final int N = 0x80_00_00_00;

    public ArmV3Cpu(Registers registers) {
        reg = registers;
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

    public void execute(OpCode opcode) throws UndefinedOpcodeException {
        if (opcode == null)
            throw new UndefinedOpcodeException("NULL");
        reg.step();
        if (opcode.instruction == Instructions.B) {
            reg.step();
            if (reg.canExecute(opcode.condition)) {
                branch(opcode.offset);
            }
        } else if (opcode.instruction == Instructions.CMP) {
            if (reg.canExecute(opcode.condition))
                compare(opcode);
        } else if (opcode.instruction == Instructions.MOV) {
            if (reg.canExecute(opcode.condition))
                move(opcode);
        }
    }

    private void move(OpCode opcode) {
        if (opcode.hasImmediate) {
            int flags = 0;
            int result = opcode.immediate;
//            System.out.println("result = " + Integer.toUnsignedString((int) result, 16));
            if (((result >> 31) & 1) != 0)
                flags |= N;
            if (result == 0)
                flags |= Z;
            reg.setReg(opcode.regDest, result);
            if (opcode.canChangePsr())
                reg.setPSR(Registers.CPSR, flags);
        }
    }

    private void compare(OpCode opcode) {
        if (opcode.hasImmediate) {
            int flags = 0;
            long before = reg.getReg(opcode.regNo);
            long result = before - (long) opcode.immediate;
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

    public void setPc(int pc) {
        reg.setReg(PC, pc);
    }
}
