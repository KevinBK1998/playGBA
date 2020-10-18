package com.kbkapps.playgba.cpu;

public class OpCode {
    Instructions instruction;
    Flags condition;
    int data;

    public OpCode(Instructions opcode, Flags cond, int data) {
        instruction = opcode;
        condition = cond;
        this.data = data;
    }

    @Override
    public String toString() {
        return instruction.toString() + " " + condition.toString() + " 0x" + Integer.toUnsignedString(data, 16);
    }
}
