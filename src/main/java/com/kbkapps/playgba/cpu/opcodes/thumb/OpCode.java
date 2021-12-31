package com.kbkapps.playgba.cpu.opcodes.thumb;

import com.kbkapps.playgba.cpu.UndefinedOpcodeException;
import com.kbkapps.playgba.cpu.constants.Instructions;

public class OpCode {
    Instructions instruction;
    byte regDest;
    byte immediate;

    public OpCode(Instructions instructions, byte regDest, byte immediate) {
        //Thumb 3
        this.instruction = instructions;
        this.regDest = regDest;
        this.immediate = immediate;
    }

    public OpCode(Instructions opcode, byte regDest) {
        instruction = opcode;
        this.regDest = regDest;
    }

    public Instructions getInstruction() {
        return instruction;
    }

    public byte getRegDest() {
        return regDest;
    }

    public byte getImmediate() {
        return immediate;
    }

    public static OpCode decodeOpcode(short opcodeEncoded) throws UndefinedOpcodeException {
        System.out.println("Decoding: " + Integer.toUnsignedString(opcodeEncoded, 16));
        if (((opcodeEncoded >> 13) & 7) == 1)
            if (((opcodeEncoded >> 11) & 3) == 0)
                return new OpCode(Instructions.MOVS, (byte) ((opcodeEncoded >> 8) & 7), (byte) (opcodeEncoded & 0xFF));
        if (((opcodeEncoded >> 11) & 0x1f) == 0b01001)
            return new OpCode(Instructions.LDR_PC, (byte) ((opcodeEncoded >> 8) & 7), (byte) (opcodeEncoded & 0xFF));
        if (((opcodeEncoded >> 11) & 0x1f) == 0b00011)
            return ArithmeticLogical.decodeOpcode(opcodeEncoded);
        if (((opcodeEncoded >> 12) & 0xf) == 0b0101)
            return SingleDataTransfer.decodeOpcode(opcodeEncoded);
        throw new UndefinedOpcodeException(Integer.toHexString(opcodeEncoded));
    }

    @Override
    public String toString() {
        return instruction.toString() + " " + getRegName(regDest) + ", 0x" + Integer.toHexString(immediate);
    }

    protected final String getRegName(int index) {
        return index < 13 ? ("r" + index) : (index == 13 ? "sp" : (index == 14 ? "lr" : "pc"));
    }
}
