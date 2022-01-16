package com.kbkapps.playgba.cpu.opcodes.thumb;

import com.kbkapps.playgba.cpu.constants.Instructions;

public class AddSP extends OpCode {
    byte offset;

    public byte getOffset() {
        return offset;
    }

    public AddSP(byte off) {
        super(Instructions.ADD_SP);
        offset = off;
    }

    @Override
    public String toString() {
        return String.format("%s, %c0x%x", instruction, offset > 0 ? '+' : '-', offset & 0x7F);
    }
}
