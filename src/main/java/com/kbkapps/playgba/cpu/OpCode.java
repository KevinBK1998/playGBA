package com.kbkapps.playgba.cpu;

public class OpCode {
    Instructions instruction;
    Flags condition;
    int data;
    boolean immediate;

    public OpCode(Instructions opcode, Flags cond, int data) {
        this(opcode, cond, data, false);
    }

    public OpCode(Instructions opcode, Flags cond, int data, boolean immediate) {
        instruction = opcode;
        condition = cond;
        this.data = data;
        this.immediate = immediate;
    }

    @Override
    public String toString() {
        if (instruction == Instructions.B)
            return instruction.toString() + " " + condition.toString() + " 0x" + Integer.toUnsignedString(data, 16);
        int regNo = (data >> 16) & 0xF;
        System.out.println("regNo = " + regNo);
        String reg = regNo < 13 ? ("r" + regNo) : (regNo == 13 ? "sp" : (regNo == 14 ? "lr" : "pc"));
        int shift = (data >> 4) & 0xF;
        System.out.println("shift=" + shift);
        int immediate = data & 0xF;
        System.out.println("immediate = " + immediate);
//        if (instruction == Instructions.CMP)
        return instruction.toString() + " " + condition.toString() + " r" + reg + " 0x" + Integer.toUnsignedString(data, 16);
    }
}
