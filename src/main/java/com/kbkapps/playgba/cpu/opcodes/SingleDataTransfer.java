package com.kbkapps.playgba.cpu.opcodes;

import com.kbkapps.playgba.cpu.constants.Flags;
import com.kbkapps.playgba.cpu.constants.Instructions;

public class SingleDataTransfer extends OpCode {
    boolean preFlag;
    boolean addFlag;
    boolean sizeOfImmediate;

    public boolean shouldAddOffsetBeforeTransfer() {
        return preFlag;
    }

    public boolean shouldAdd() {
        return addFlag;
    }

    public boolean isSizeOfImmediate() {
        return sizeOfImmediate;
    }

    SingleDataTransfer(Instructions opcode, Flags cond, int flags, int data) {
        super(opcode, cond, flags, data);
        immediateFlag = ((flags >> 3) & 1) == 0;//I
        preFlag = ((flags >> 2) & 1) != 0;//P
        addFlag = ((flags >> 1) & 1) != 0;//U
        sizeOfImmediate = (flags & 1) != 0;//B
        immediate = data & 0xF_FF;
//        System.out.println("immediate (12-bit)= 0x" + Integer.toUnsignedString(immediate, 16));
    }

    @Override
    public String toString() {
        return condition.toString() + " " + instruction.toString() + (sizeOfImmediate ? " byte " : " word ") + getRegName(regDest) + ", [" + getRegName(regNo) + (preFlag ? " " : "] ") + (addFlag ? "+" : "-") + " 0x" + Integer.toUnsignedString(immediate, 16) + (preFlag ? "]" : "");
    }
}
