package com.kbkapps.playgba.cpu.opcodes.thumb;

import com.kbkapps.playgba.cpu.UndefinedOpcodeException;
import com.kbkapps.playgba.cpu.constants.Instructions;

public class OpCode {
    Instructions instructions;
    byte regDest;
    byte immediate;

    public OpCode(Instructions instructions, byte regDest, byte immediate) {
        //Thumb 3
        this.instructions = instructions;
        this.regDest = regDest;
        this.immediate = immediate;
    }

    public static OpCode decodeOpcode(short opcodeEncoded) throws UndefinedOpcodeException {
        if (((opcodeEncoded >> 13) & 7) == 1)
            if (((opcodeEncoded >> 11) & 3) == 0)
                return new OpCode(Instructions.MOV, (byte) ((opcodeEncoded >> 8) & 3), (byte) (opcodeEncoded & 0xFF));
        throw new UndefinedOpcodeException(Integer.toHexString(opcodeEncoded));
    }

    @Override
    public String toString() {
        return instructions.toString() + " r" + regDest + ", 0x" + Integer.toHexString(immediate);
    }
}
