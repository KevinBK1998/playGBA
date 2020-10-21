package com.kbkapps.playgba.cpu;

import static com.kbkapps.playgba.cpu.Flags.*;

public class ArmV3Cpu {
    public static final Flags HS = CS;
    public static final Flags LO = CC;
    public static final int PC = 15;
    Registers reg;

    public ArmV3Cpu(Registers registers) {
        reg = registers;
    }

    public void runOpcode(String opCode, String condition, int label) {
        Flags cond = Flags.valueOf(condition);
        if (opCode.equals("B"))
            if (reg.canExecute(cond))
                branch(label);
    }

    private void branch(int label) {
        reg.setReg(PC, (int) (Integer.toUnsignedLong(reg.getReg(PC)) + label * 4));
    }

    public OpCode decode(int opcodeEncoded) {
        reg.step();
        Flags cond = values()[(opcodeEncoded >> 28) & 0xF];
        if (((opcodeEncoded >> 25) & 0xF) == 0b101)//Branching
            if (((opcodeEncoded >> 24) & 0x1) == 0b0) {//Branch
                reg.step();
                Instructions opcode = Instructions.B;
                int jumpBy = opcodeEncoded & 0x00_FF_FF_FF;
                return new OpCode(opcode, cond, jumpBy);
            }
        if (((opcodeEncoded >> 26) & 0x3) == 0b00)//ALU
            if (((opcodeEncoded >> 25) & 0x1) != 0)//Immediate
                if (((opcodeEncoded >> 21) & 0xF) == 0xA && ((opcodeEncoded >> 20) & 0x1) != 0) {//Compare
                    Instructions opcode = Instructions.CMP;
                    int data = opcodeEncoded & 0xF_FF_FF;
                    return new OpCode(opcode, cond, data, true);
                }
        return null;
    }

    private void compareWithImmediate(int opcodeEncoded) {
        int regNo = (opcodeEncoded >> 16) & 0xF;
        System.out.println("regNo = " + regNo);
        int shift = (opcodeEncoded >> 4) & 0xF;
        System.out.println("shift=" + shift);
        int immediate = opcodeEncoded & 0xF;
        System.out.println("immediate = " + immediate);
    }

    public OpCode decode(byte[] opCodes) {
        reg.step();
        Flags cond = values()[Byte.toUnsignedInt(opCodes[0]) >> 4];
        Instructions opcode = null;
        int data;
        boolean immediate;
        if ((opCodes[0] & 0xF) == 0b1010) {
            opcode = Instructions.B;
            data = 65025 * opCodes[1] + 255 * opCodes[2] + opCodes[3];
            return new OpCode(opcode, cond, data);
        } else
//            if ((opCodes[0] & 0xC) == 0b0000)
        {
            immediate = (opCodes[0] & 0x2) != 0;
//            if(((opCodes[0] & 0x1) == 0b0001)&& ((Byte.toUnsignedInt(opCodes[1]) >> 4)&0xE == 0b0100)) {
            opcode = Instructions.CMP;
            data = 0;
            return new OpCode(opcode, cond, data);
//            } else throw new IndexOutOfBoundsException(String.valueOf((Byte.toUnsignedInt(opCodes[1]) >> 4)));
        }
    }

    public void execute(OpCode opcode) {
        if (opcode.instruction == Instructions.B)
            if (reg.canExecute(opcode.condition))
                branch(opcode.data);
    }
}
