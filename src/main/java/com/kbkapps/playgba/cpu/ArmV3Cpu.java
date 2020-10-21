package com.kbkapps.playgba.cpu;

import com.kbkapps.playgba.cpu.constants.Flags;
import com.kbkapps.playgba.cpu.constants.Instructions;

import static com.kbkapps.playgba.cpu.constants.Flags.*;

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
        reg.setReg(PC, (int) (Integer.toUnsignedLong(reg.getReg(PC)) + label * 4));
    }

    public OpCode decode(int opcodeEncoded) {
        reg.step();
        Flags cond = values()[(opcodeEncoded >> 28) & 0xF];
        if (((opcodeEncoded >> 25) & 0xF) == 0b101)//Branching
            if (((opcodeEncoded >> 24) & 0x1) == 0b0) {//Branch
                reg.step();
                Instructions opcode = Instructions.B;
                int jumpBy = opcodeEncoded & 0x00_FF_FF_FF;
                return new OpCode(opcode, cond, jumpBy);
            }
        if (((opcodeEncoded >> 26) & 0x3) == 0b00)//ALU
            if (((opcodeEncoded >> 25) & 0x1) != 0)//Immediate
                if (((opcodeEncoded >> 21) & 0xF) == 0xA) {//Compare
                    checkIfPsrCanChange(opcodeEncoded);
                    Instructions opcode = Instructions.CMP;
                    int data = opcodeEncoded & 0xF_FF_FF;
                    return new OpCode(opcode, cond, true, data);
                } else if (((opcodeEncoded >> 21) & 0xF) == 0xD) {
                    Instructions opcode = Instructions.MOV;
                    int data = opcodeEncoded & 0xF_FF_FF;
                    return new OpCode(opcode, cond, true, data);
                }
        return null;
    }

    private void checkIfPsrCanChange(int opcodeEncoded) {
        if (!changePSR(opcodeEncoded)) throw new IndexOutOfBoundsException("CPSR must be written");
    }

    private boolean changePSR(int opcodeEncoded) {
        //only for ALU opcodes 8-B
        return ((opcodeEncoded >> 20) & 0x1) != 0;
    }

    public void execute(OpCode opcode) {
        if (opcode.instruction == Instructions.B) {
            if (reg.canExecute(opcode.condition))
                branch(opcode.offset);
        } else if (opcode.instruction == Instructions.CMP) {
            if (reg.canExecute(opcode.condition))
                compare(opcode);
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
        }
    }
}
