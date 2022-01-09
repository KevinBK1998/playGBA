package com.kbkapps.playgba.cpu.opcodes.thumb;

import com.kbkapps.playgba.cpu.UndefinedOpcodeException;
import com.kbkapps.playgba.cpu.constants.Flags;
import com.kbkapps.playgba.cpu.constants.Instructions;

public class Branch extends OpCode {
    Flags condition;
    byte off;

    public Branch(Flags cond, byte offset) {
        super(Instructions.B);
        condition = cond;
        off = offset;
    }

    public static Branch decodeOpcode(short opcodeEncoded) throws UndefinedOpcodeException {
        int opcode = (opcodeEncoded >> 8) & 0xf;
        int offset = opcodeEncoded & 0xff;
        switch (opcode) {
            case 0xB:
                return new Branch(Flags.LT, (byte) offset);
            default:
                throw new UndefinedOpcodeException(opcodeEncoded);
        }
    }

    public Flags getCondition() {
        return condition;
    }

    public byte getOffset() {
        return off;
    }

    @Override
    public String toString() {
        return instruction.toString() + " " + condition + ", " + off;
    }
}
