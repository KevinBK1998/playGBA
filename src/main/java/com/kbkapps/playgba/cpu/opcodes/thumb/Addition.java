package com.kbkapps.playgba.cpu.opcodes.thumb;

import com.kbkapps.playgba.cpu.UndefinedOpcodeException;
import com.kbkapps.playgba.cpu.constants.Instructions;

public class Addition extends OpCode {
    int regN;
    int regS;
    boolean isImmediate;

    public Addition(Instructions instructions, int opcodeEncoded) {
        super(instructions, (byte) (opcodeEncoded & 7));
        regS = (byte) ((opcodeEncoded >> 3) & 7);
        regN = (byte) ((opcodeEncoded >> 6) & 7);
    }

    public Addition(Instructions instructions, int opcodeEncoded, boolean isImm) {
        super(instructions, (byte) (opcodeEncoded & 7), (byte) ((opcodeEncoded >> 6) & 7));
        regS = (byte) ((opcodeEncoded >> 3) & 7);
        isImmediate = isImm;
    }

    public static Addition decodeOpcode(short opcodeEncoded) throws UndefinedOpcodeException {
        int opcode = (opcodeEncoded >> 9) & 3;
        int data = opcodeEncoded & 0x1ff;
        if (opcode == 0)
            return new Addition(Instructions.ADD, data);
        else if (opcode == 2)
            return new Addition(Instructions.ADD, data, true);
        else
            throw new UndefinedOpcodeException(Integer.toHexString(opcodeEncoded));
    }

    @Override
    public String toString() {
        return instruction.toString() + " " + getRegName(regDest) + ", " + getRegName(regS) + ", " + (isImmediate ? ("0x" + immediate) : getRegName(regN));
    }

    public boolean isImmediate() {
        return isImmediate;
    }

    public int getRegNext() {
        return regN;
    }

    public int getRegSource() {
        return regS;
    }
}
