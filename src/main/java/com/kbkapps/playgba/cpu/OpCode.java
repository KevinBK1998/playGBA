package com.kbkapps.playgba.cpu;

import com.kbkapps.playgba.cpu.constants.Flags;
import com.kbkapps.playgba.cpu.constants.Instructions;

public class OpCode {
    Instructions instruction;
    Flags condition;
    int offset;
    int immediate;
    boolean hasImmediate;
    int regNo;

    public OpCode(Instructions opcode, Flags cond, int offset) {
        instruction = opcode;
        condition = cond;
        this.offset = offset;
        hasImmediate = false;
    }

    public OpCode(Instructions opcode, Flags cond, boolean hasImmediate, int data) {
        instruction = opcode;
        condition = cond;
        regNo = (data >> 16) & 0xF;
        this.hasImmediate = hasImmediate;
        int shift = (data >> 4) & 0xF;
//        System.out.println("shift = " + shift);
        int immediate = data & 0xF;
//        System.out.println("immediate (8-bit)= " + immediate);
        this.immediate = immediate >> shift;

    }

    @Override
    public String toString() {
        if (instruction == Instructions.B)
            return instruction.toString() + " " + condition.toString() + " 0x" + Integer.toUnsignedString(offset, 16);
        String reg = regNo < 13 ? ("r" + regNo) : (regNo == 13 ? "sp" : (regNo == 14 ? "lr" : "pc"));
//        if (instruction == Instructions.CMP)
        return instruction.toString() + " " + condition.toString() + " " + reg + " 0x" + Integer.toUnsignedString(offset, 16);
    }
}
