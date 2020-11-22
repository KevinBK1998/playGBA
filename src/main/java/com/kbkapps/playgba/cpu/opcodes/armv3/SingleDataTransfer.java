package com.kbkapps.playgba.cpu.opcodes.armv3;

import com.kbkapps.playgba.cpu.constants.Flags;
import com.kbkapps.playgba.cpu.constants.Instructions;

import static com.kbkapps.playgba.cpu.constants.Flags.values;

public class SingleDataTransfer extends OpCode {
    boolean preFlag;
    boolean addFlag;

    boolean byteTransfer;

    SingleDataTransfer(Instructions opcode, Flags cond, int flags, int data) {
        super(opcode, cond, flags, data);
        preFlag = ((flags >> 2) & 1) != 0;//P
        addFlag = ((flags >> 1) & 1) != 0;//U
        byteTransfer = (flags & 1) != 0;//B
        immediate = data & 0xF_FF;
//        System.out.println("immediate (12-bit)= 0x" + Integer.toUnsignedString(immediate, 16));
    }

    public boolean shouldAddOffsetBeforeTransfer() {
        return preFlag;
    }

    public boolean shouldAdd() {
        return addFlag;
    }

    public boolean isByteTransfer() {
        return byteTransfer;
    }

    public static SingleDataTransfer decodeOpcode(int opcodeEncoded) {
        Flags cond = values()[(opcodeEncoded >> 28) & 0xF];
        int flags = ((opcodeEncoded >> 22) & 0xF);
        Instructions opcode;
        int data = opcodeEncoded & 0xF_FF_FF;
        if (((opcodeEncoded >> 20) & 1) != 0) // Load: LDR{cond}{B}{T} Rd,<Address>   ;Rd=[Rn+/-<offset>]
            opcode = Instructions.LDR;
        else // STR{cond}{B}{T} Rd,<Address>   ;[Rn+/-<offset>]=Rd
            opcode = Instructions.STR;
        return new SingleDataTransfer(opcode, cond, flags, data);
    }

    @Override
    public String toString() {
        return condition.toString() + " " + instruction.toString() + (byteTransfer ? " byte " : " word ") + getRegName(regDest) + ", [" + getRegName(regNo) + (preFlag ? " " : "] ") + (addFlag ? "+" : "-") + " 0x" + Integer.toUnsignedString(immediate, 16) + (preFlag ? "]" : "");
    }
}
