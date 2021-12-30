package com.kbkapps.playgba.cpu.opcodes.thumb;

import com.kbkapps.playgba.cpu.constants.Instructions;

public class SingleDataTransfer extends OpCode {
    boolean byteTransfer;
    int baseReg;
    int offsetReg;

    public SingleDataTransfer(Instructions opcode, int data) {
        super(opcode, data);
        byteTransfer = ((data >> 9) & 1) != 0;//B
        baseReg = ((data >> 3) & 7);
        offsetReg = ((data >> 6) & 7);
    }

    public static SingleDataTransfer decodeOpcode(short opcodeEncoded) {
        int opcode = (opcodeEncoded >> 10) & 3;
        int data = opcodeEncoded & 0x3ff;
        if (opcode < 2)
            return new SingleDataTransfer(Instructions.STR, data);
        else
            return new SingleDataTransfer(Instructions.LDR, data);
    }

    public int getBaseReg() {
        return baseReg;
    }

    public int getOffsetReg() {
        return offsetReg;
    }

    @Override
    public String toString() {
        return instruction.toString() + (byteTransfer ? " byte " : " word ") + getRegName(regDest) + ", [" + getRegName(baseReg) + "+" + getRegName(offsetReg) + "]";
    }
}
