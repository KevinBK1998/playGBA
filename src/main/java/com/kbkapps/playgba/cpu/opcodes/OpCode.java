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
    boolean changePSR;

    public OpCode(Instructions opcode, Flags cond, boolean immediateFlag, boolean canChangePSR, int data) {
        instruction = opcode;
        condition = cond;
        regNo = (data >> 16) & 0xF;
        regDest = (data >> 12) & 0xF;
        this.immediateFlag = immediateFlag;
        changePSR = canChangePSR;
        int shift = (data >> 8) & 0xF;
//        System.out.println("shift = " + shift);
        int immediate = data & 0xF;
//        System.out.println("immediate (8-bit)= " + immediate);
        this.immediate = Integer.rotateRight(immediate, 2 * shift);
    }

    public OpCode(Instructions opcode, Flags cond, boolean immediateFlag, int data) {
        this(opcode, cond, immediateFlag, true, data);
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
            if (((opcodeEncoded >> 25) & 0x1) != 0)//Immediate
                if (((opcodeEncoded >> 21) & 0xF) == 0x9) {//Test Exclusive
                    return getVoidAluOpcode(opcodeEncoded, cond, Instructions.TEQ);
                } else if (((opcodeEncoded >> 21) & 0xF) == 0xA) {//Compare
                    return getVoidAluOpcode(opcodeEncoded, cond, Instructions.CMP);
                } else if (((opcodeEncoded >> 21) & 0xF) == 0xD) {//Move
                    Instructions opcode = Instructions.MOV;
                    int data = opcodeEncoded & 0xF_FF_FF;
                    return new OpCode(opcode, cond, true, shouldChangePSR(opcodeEncoded), data);
                }
        if (((opcodeEncoded >> 26) & 0x3) == 0b01)//Single Data Transfer
        {
            int flags = ((opcodeEncoded >> 22) & 0xF);
            if (((opcodeEncoded >> 20) & 1) != 0) {//Load: LDR{cond}{B}{T} Rd,<Address>   ;Rd=[Rn+/-<offset>]
                Instructions opcode = Instructions.LDR;
                int data = opcodeEncoded & 0xF_FF_FF;
                return new SingleDataTransfer(opcode, cond, flags, data);
            }
        }
        throw new UndefinedOpcodeException(opcodeEncoded);
    }

    private static OpCode getVoidAluOpcode(int opcodeEncoded, Flags cond, Instructions opcode) {
        //only for ALU opcodes 8-B
        if (!shouldChangePSR(opcodeEncoded)) throw new IndexOutOfBoundsException("CPSR must be written");
        int data = opcodeEncoded & 0xF_FF_FF;
        return new OpCode(opcode, cond, true, data);
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

    public OpCode(Instructions opcode, Flags cond, int offset) {
        instruction = opcode;
        condition = cond;
        this.offset = offset;
    }

    public boolean hasImmediate() {
        return immediateFlag;
    }

    public OpCode(Instructions opcode, Flags cond, int flags, int data) {
        instruction = opcode;
        condition = cond;
        regNo = (data >> 16) & 0xF;
        regDest = (data >> 12) & 0xF;
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

    private static boolean shouldChangePSR(int opcodeEncoded) {
        return ((opcodeEncoded >> 20) & 0x1) != 0;
    }

    @Override
    public String toString() {
        if (instruction == Instructions.B)
            return condition.toString() + " " + instruction.toString() + " 0x" + Integer.toUnsignedString(offset, 16);
        if (instruction == Instructions.TEQ)
            return condition.toString() + " " + instruction.toString() + " " + getRegName(regNo) + " 0x" + Integer.toUnsignedString(immediate, 16);
        if (instruction == Instructions.CMP)
            return condition.toString() + " " + instruction.toString() + " " + getRegName(regNo) + " 0x" + Integer.toUnsignedString(immediate, 16);
        if (instruction == Instructions.MOV)
            return condition.toString() + " " + instruction.toString() + " " + getRegName(regDest) + " 0x" + Integer.toUnsignedString(immediate, 16);
        return null;
    }

    protected final String getRegName(int index) {
        return index < 13 ? ("r" + index) : (index == 13 ? "sp" : (index == 14 ? "lr" : "pc"));
    }

    public boolean canChangePsr() {
        return changePSR;
    }
}
