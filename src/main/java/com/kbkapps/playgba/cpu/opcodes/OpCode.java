package com.kbkapps.playgba.cpu.opcodes;

import com.kbkapps.playgba.cpu.UndefinedOpcodeException;
import com.kbkapps.playgba.cpu.constants.Flags;
import com.kbkapps.playgba.cpu.constants.Instructions;

import static com.kbkapps.playgba.cpu.constants.Flags.values;

public class OpCode {
    Instructions instruction;
    Flags condition;
    int offset;
    int immediate;
    boolean immediateFlag;
    int regNo;
    int regDest;

    public OpCode(Instructions opcode, Flags cond, int flags, int data) {
        instruction = opcode;
        condition = cond;
        immediateFlag = ((flags >> 3) & 1) == 0;//I
        regNo = (data >> 16) & 0xF;
        regDest = (data >> 12) & 0xF;
    }

    public OpCode(Instructions opcode, Flags cond, int offset) {
        instruction = opcode;
        this.offset = offset;
        condition = cond;
    }

    public OpCode(Instructions opcode, Flags cond, boolean immediateFlag) {
        instruction = opcode;
        condition = cond;
        this.immediateFlag = immediateFlag;
    }

    public static OpCode decodeOpcode(int opcodeEncoded) throws UndefinedOpcodeException {
        System.out.println("Decoding: " + Integer.toUnsignedString(opcodeEncoded, 16));
        Flags cond = values()[(opcodeEncoded >> 28) & 0xF];
        if (((opcodeEncoded >> 25) & 0xF) == 0b101)//Branching
            if (((opcodeEncoded >> 24) & 0x1) == 0b0) {//Branch
                Instructions opcode = Instructions.B;
                int jumpBy = opcodeEncoded & 0x00_FF_FF_FF;
                return new OpCode(opcode, cond, jumpBy);
            }
        if (((opcodeEncoded >> 26) & 0x3) == 0b00)//ALU
            return ArithmeticLogical.decodeOpcode(cond, opcodeEncoded);
        if (((opcodeEncoded >> 26) & 0x3) == 0b01)//Single Data Transfer
        {
            int flags = ((opcodeEncoded >> 22) & 0xF);
            if (((opcodeEncoded >> 20) & 1) != 0) {//Load: LDR{cond}{B}{T} Rd,<Address>   ;Rd=[Rn+/-<offset>]
                Instructions opcode = Instructions.LDR;
                int data = opcodeEncoded & 0xF_FF_FF;
                return new SingleDataTransfer(opcode, cond, flags, data);
            }
        }
        System.out.println("cond = " + cond);
        System.out.println("rest of opcodeEncoded = " + Integer.toUnsignedString(opcodeEncoded & 0xF_FF_FF_FF, 16));
        throw new UndefinedOpcodeException(opcodeEncoded);
    }

    public Instructions getInstruction() {
        return instruction;
    }

    public Flags getCondition() {
        return condition;
    }

    public int getOffset() {
        return offset;
    }

    public int getImmediate() {
        return immediate;
    }

    public boolean hasImmediate() {
        return immediateFlag;
    }

    @Override
    public boolean equals(Object obj) {
        if (obj instanceof OpCode)
            return this.toString().equalsIgnoreCase(obj.toString());
        return false;
    }

    public int getRegNo() {
        return regNo;
    }

    public int getRegDest() {
        return regDest;
    }


    @Override
    public String toString() {
        if (instruction == Instructions.B)
            return condition.toString() + " " + instruction.toString() + " 0x" + Integer.toUnsignedString(offset, 16);
        return null;
    }

    protected final String getRegName(int index) {
        return index < 13 ? ("r" + index) : (index == 13 ? "sp" : (index == 14 ? "lr" : "pc"));
    }
}
