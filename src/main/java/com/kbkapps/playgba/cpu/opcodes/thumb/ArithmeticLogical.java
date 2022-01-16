package com.kbkapps.playgba.cpu.opcodes.thumb;

import com.kbkapps.playgba.cpu.UndefinedOpcodeException;
import com.kbkapps.playgba.cpu.constants.Instructions;

public class ArithmeticLogical extends OpCode {
    int regS;

    public ArithmeticLogical(Instructions instructions, int opcodeEncoded) {
        super(instructions, (byte) (opcodeEncoded & 7));
        regS = (byte) ((opcodeEncoded >> 3) & 7);
    }

    public static ArithmeticLogical decodeOpcode(byte opcode, int restOfOpcodeEncoded) throws UndefinedOpcodeException {
        if (opcode == 0xf)
            return new ArithmeticLogical(Instructions.MVN, restOfOpcodeEncoded);
        else
            throw new UndefinedOpcodeException(opcode + " " + restOfOpcodeEncoded);
    }

    public int getRegSource() {
        return regS;
    }

    @Override
    public String toString() {
        return instruction.toString() + " " + getRegName(regDest) + ", " + getRegName(regS);
    }
}
