package com.kbkapps.playgba.cpu;

import com.kbkapps.playgba.cpu.constants.Flags;
import com.kbkapps.playgba.cpu.constants.Instructions;

import static com.kbkapps.playgba.cpu.constants.Flags.values;

public class OpCode {
    Instructions instruction;
    Flags condition;
    int offset;
    int immediate;
    boolean hasImmediate;
    int regNo;
    int regDest;
    boolean changePSR;

    public OpCode(Instructions opcode, Flags cond, int offset) {
        instruction = opcode;
        condition = cond;
        this.offset = offset;
        hasImmediate = false;
    }

    public OpCode(Instructions opcode, Flags cond, boolean hasImmediate, boolean canChangePSR, int data) {
        instruction = opcode;
        condition = cond;
        regNo = (data >> 16) & 0xF;
        regDest = (data >> 12) & 0xF;
        this.hasImmediate = hasImmediate;
        changePSR = canChangePSR;
        int shift = (data >> 8) & 0xF;
//        System.out.println("shift = " + shift);
        int immediate = data & 0xF;
//        System.out.println("immediate (8-bit)= " + immediate);
        this.immediate = Integer.rotateRight(immediate, 2 * shift);
    }

    public OpCode(Instructions opcode, Flags cond, boolean hasImmediate, int data) {
        this(opcode, cond, hasImmediate, true, data);
    }

    static OpCode decodeOpcode(int opcodeEncoded) {
        Flags cond = values()[(opcodeEncoded >> 28) & 0xF];
        if (((opcodeEncoded >> 25) & 0xF) == 0b101)//Branching
            if (((opcodeEncoded >> 24) & 0x1) == 0b0) {//Branch
                Instructions opcode = Instructions.B;
                int jumpBy = opcodeEncoded & 0x00_FF_FF_FF;
                return new OpCode(opcode, cond, jumpBy);
            }
        if (((opcodeEncoded >> 26) & 0x3) == 0b00)//ALU
            if (((opcodeEncoded >> 25) & 0x1) != 0)//Immediate
                if (((opcodeEncoded >> 21) & 0xF) == 0xA) {//Compare
                    checkIfPsrCanChange(opcodeEncoded);
                    Instructions opcode = Instructions.CMP;
                    int data = opcodeEncoded & 0xF_FF_FF;
                    return new OpCode(opcode, cond, true, data);
                } else if (((opcodeEncoded >> 21) & 0xF) == 0xD) {//Move
                    Instructions opcode = Instructions.MOV;
                    int data = opcodeEncoded & 0xF_FF_FF;
                    return new OpCode(opcode, cond, true, shouldChangePSR(opcodeEncoded), data);
                }
        return null;
    }

    private static void checkIfPsrCanChange(int opcodeEncoded) {
        //only for ALU opcodes 8-B
        if (!shouldChangePSR(opcodeEncoded)) throw new IndexOutOfBoundsException("CPSR must be written");
    }

    private static boolean shouldChangePSR(int opcodeEncoded) {
        return ((opcodeEncoded >> 20) & 0x1) != 0;
    }

    @Override
    public String toString() {
        if (instruction == Instructions.B)
            return instruction.toString() + " " + condition.toString() + " 0x" + Integer.toUnsignedString(offset, 16);
        if (instruction == Instructions.CMP)
            return instruction.toString() + " " + condition.toString() + " " + getRegName(regNo) + " 0x" + Integer.toUnsignedString(immediate, 16);
//        if(instruction==Instructions.MOV)
        return instruction.toString() + " " + condition.toString() + " " + getRegName(regDest) + " 0x" + Integer.toUnsignedString(immediate, 16);
    }

    private String getRegName(int index) {
        return index < 13 ? ("r" + index) : (index == 13 ? "sp" : (index == 14 ? "lr" : "pc"));
    }

    public boolean canChangePsr() {
        return changePSR;
    }
}
