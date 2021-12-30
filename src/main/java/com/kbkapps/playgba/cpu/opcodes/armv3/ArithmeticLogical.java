package com.kbkapps.playgba.cpu.opcodes.armv3;

import com.kbkapps.playgba.cpu.UndefinedOpcodeException;
import com.kbkapps.playgba.cpu.constants.Flags;
import com.kbkapps.playgba.cpu.constants.Instructions;

import static com.kbkapps.playgba.cpu.constants.Flags.values;

public class ArithmeticLogical extends OpCode {
    boolean changePSR;
    private boolean isSavedPsr;
    int regM;
    int mask;

    public int getRegSource() {
        return regM;
    }

    public int getMask() {
        return mask;
    }

    public ArithmeticLogical(boolean isSavedPsr, Instructions opcode, Flags cond, int flags, int mReg) {
        super(opcode, cond, false);
        this.isSavedPsr = isSavedPsr;
        mask = 0;
        for (int i = 3; i >= 0; i--) {
            mask = mask << 8;
            boolean bit = ((flags >> i) & 1) != 0;
            if (bit)
                mask += 0xFF;
        }
        regM = mReg;
    }

    public char getPsr() {
        return isSavedPsr ? 'S' : 'C';
    }

    public ArithmeticLogical(Instructions opcode, Flags cond, boolean immediateFlag, boolean canChangePsr, int opcodeEncoded) {
        super(opcode, cond, immediateFlag);
        regNo = (opcodeEncoded >> 16) & 0xF;
//        System.out.println("regNo = " + regNo);
        regDest = (opcodeEncoded >> 12) & 0xF;
//        System.out.println("regDest = " + regDest);
        this.immediateFlag = immediateFlag;
        changePSR = canChangePsr;
        int shift = (opcodeEncoded >> 8) & 0xF;
//        System.out.println("shift = " + shift);
        int immediate = opcodeEncoded & 0xFF;
//        System.out.println("immediate (8-bit)= " + immediate);
        this.immediate = Integer.rotateRight(immediate, 2 * shift);
    }

    public ArithmeticLogical(Instructions opcode, Flags cond, boolean immediateFlag, int opcodeEncoded) {
        this(opcode, cond, immediateFlag, true, opcodeEncoded);
    }

    public ArithmeticLogical(boolean isSavedPsr, Instructions opcode, Flags cond, int opcodeEncoded) {
        this(opcode, cond, false, false, opcodeEncoded);
        this.isSavedPsr = isSavedPsr;
    }

    public static ArithmeticLogical decodeOpcode(int opcodeEncoded) throws UndefinedOpcodeException {
        Flags cond = values()[(opcodeEncoded >> 28) & 0xF];
        Instructions opcode = null;
        switch ((opcodeEncoded >> 21) & 0xF) {
            case 0x4:
                opcode = Instructions.ADD;
                break;
            case 0x9:
                opcode = Instructions.TEQ;
                break;
            case 0xA:
                opcode = Instructions.CMP;
                break;
            case 0xC:
                opcode = Instructions.ORR;
                break;
            case 0xD:
                opcode = Instructions.MOV;
                break;
            default:
//                System.out.println("cond = " + cond);
                System.out.println("rest of opcodeEncoded(ALU) = " + Integer.toUnsignedString(opcodeEncoded & 0xF_FF_FF_FF, 16));
        }
        boolean shouldChangePsr = ((opcodeEncoded >> 20) & 0x1) != 0;
        if (((opcodeEncoded >> 25) & 0x1) != 0) {//Immediate
            int data = opcodeEncoded & 0xF_FF_FF;
            if (((opcodeEncoded >> 23) & 0x3) == 0b10) {
                if (!shouldChangePsr)
                    throw new IndexOutOfBoundsException("CPSR must be written");
                if (opcode != null)
                    return new ArithmeticLogical(opcode, cond, true, data);
            } else {
                if (opcode != null)
                    return new ArithmeticLogical(opcode, cond, true, shouldChangePsr, data);
            }
        } else {//Non-immediate
            if (((opcodeEncoded >> 23) & 0x3) == 0b10) {
                if (shouldChangePsr) {
                    throw new IndexOutOfBoundsException("return non immediate ALU");
//                return new ArithmeticLogical(opcode, cond, true, data);
                }
                boolean isSavedPsr = ((opcodeEncoded >> 22) & 0x1) != 0;
                if (((opcodeEncoded >> 21) & 0x1) == 0) {//Move PSR to Reg
                    if (((opcodeEncoded >> 16) & 0xF) == 0xF) {
                        opcode = Instructions.MRS;
                        int data = opcodeEncoded & 0xF0_00;
                        return new ArithmeticLogical(isSavedPsr, opcode, cond, data);
                    }
                } else {
                    if (((opcodeEncoded >> 12) & 0xF) == 0xF && ((opcodeEncoded >> 4) & 0xFF) == 0) {//Move Reg to PSR
                        int flags = (opcodeEncoded >> 16) & 0xF;
                        opcode = Instructions.MSR;
                        int mReg = opcodeEncoded & 0xF;
                        return new ArithmeticLogical(isSavedPsr, opcode, cond, flags, mReg);
                    }
                }
            }
        }
        System.out.println("cond = " + cond);
        System.out.println("shouldChangePsr = " + shouldChangePsr);
        System.out.println("rest of opcodeEncoded(ALUs) = " + Integer.toUnsignedString(opcodeEncoded & 0xFF_FF_FF, 16));
        throw new UndefinedOpcodeException(opcodeEncoded);
    }

    @Override
    public String toString() {
//        System.out.println("changePsr= " + changePSR);
        if (instruction == Instructions.ADD)
            return condition.toString() + " " + instruction + " " + getRegName(regDest) + ", " + getRegName(regNo) + ", 0x" + Integer.toUnsignedString(immediate, 16);
        if (instruction == Instructions.TEQ)
            return condition.toString() + " " + instruction + " " + getRegName(regNo) + ", 0x" + Integer.toUnsignedString(immediate, 16);
        if (instruction == Instructions.CMP)
            return condition.toString() + " " + instruction + " " + getRegName(regNo) + ", 0x" + Integer.toUnsignedString(immediate, 16);
        if (instruction == Instructions.ORR)
            return condition.toString() + " " + instruction + " " + getRegName(regDest) + ", " + getRegName(regNo) + ", 0x" + Integer.toUnsignedString(immediate, 16);
        if (instruction == Instructions.MOV)
            return condition.toString() + " " + instruction + " " + getRegName(regDest) + ", 0x" + Integer.toUnsignedString(immediate, 16);
        if (instruction == Instructions.MRS)
            return condition.toString() + " " + instruction + " " + getRegName(regDest) + ", " + (isSavedPsr ? "S" : "C") + "PSR";
        if (instruction == Instructions.MSR)
            return condition.toString() + " " + instruction + " " + (isSavedPsr ? "S" : "C") + "PSR" + getMaskAsString() + ", " + getRegName(regM);
        return null;
    }

    private String getMaskAsString() {
        String maskString = "_";
        if (((mask >> 24) & 1) > 0)
            maskString += "f";
        if (((mask >> 16) & 1) > 0)
            maskString += "s";
        if (((mask >> 8) & 1) > 0)
            maskString += "x";
        if ((mask & 1) > 0)
            maskString += "c";
        return maskString;
    }

    public boolean canChangePsr() {
        return changePSR;
    }
}
