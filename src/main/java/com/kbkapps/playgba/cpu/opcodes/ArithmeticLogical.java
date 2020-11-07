package com.kbkapps.playgba.cpu.opcodes;

import com.kbkapps.playgba.cpu.UndefinedOpcodeException;
import com.kbkapps.playgba.cpu.constants.Flags;
import com.kbkapps.playgba.cpu.constants.Instructions;

public class ArithmeticLogical extends OpCode {
    private boolean isSavedPsr;

    public char getPsr() {
        return isSavedPsr ? 'S' : 'C';
    }

    public ArithmeticLogical(Instructions opcode, Flags cond, boolean immediateFlag, boolean canChangePsr, int opcodeEncoded) {
        super(opcode, cond, immediateFlag, canChangePsr, opcodeEncoded);
    }

    public ArithmeticLogical(Instructions opcode, Flags cond, boolean immediateFlag, int opcodeEncoded) {
        super(opcode, cond, immediateFlag, opcodeEncoded);
    }

    public ArithmeticLogical(boolean isSavedPsr, Instructions opcode, Flags cond, int opcodeEncoded) {
        super(opcode, cond, false, false, opcodeEncoded);
        this.isSavedPsr = isSavedPsr;
    }

    private static boolean shouldChangePSR(int opcodeEncoded) {
        return ((opcodeEncoded >> 20) & 0x1) != 0;
    }

    public static ArithmeticLogical decodeOpcode(Flags cond, int opcodeEncoded) throws UndefinedOpcodeException {
        Instructions opcode = null;
        switch ((opcodeEncoded >> 21) & 0xF) {
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
                System.out.println("cond = " + cond);
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
                }
            }
        }
        System.out.println("cond = " + cond);
        System.out.println("rest of opcodeEncoded(ALU) = " + Integer.toUnsignedString(opcodeEncoded & 0xFF_FF_FF, 16));
        throw new UndefinedOpcodeException(opcodeEncoded);
    }

    @Override
    public String toString() {
        if (instruction == Instructions.TEQ)
            return condition.toString() + " " + instruction.toString() + " " + getRegName(regNo) + ", 0x" + Integer.toUnsignedString(immediate, 16);
        if (instruction == Instructions.CMP)
            return condition.toString() + " " + instruction.toString() + " " + getRegName(regNo) + ", 0x" + Integer.toUnsignedString(immediate, 16);
        if (instruction == Instructions.ORR)
            return condition.toString() + " " + instruction.toString() + " " + getRegName(regDest) + ", " + getRegName(regNo) + ", 0x" + Integer.toUnsignedString(immediate, 16);
        if (instruction == Instructions.MOV)
            return condition.toString() + " " + instruction.toString() + " " + getRegName(regDest) + ", 0x" + Integer.toUnsignedString(immediate, 16);
        if (instruction == Instructions.MRS)
            return condition.toString() + " " + instruction.toString() + " " + getRegName(regDest) + ", " + (isSavedPsr ? "S" : "A") + "PSR";
        return null;
    }
}
