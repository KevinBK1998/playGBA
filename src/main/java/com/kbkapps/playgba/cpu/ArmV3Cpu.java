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

    public OpCode decode(byte[] opCodes) {
        Instructions opcode = null;
        if ((opCodes[0] & 0xF) == 0b1010)
            opcode = Instructions.B;
        else if ((opCodes[0] & 0xC) == 0b0000) {

        }
        Flags cond = values()[Byte.toUnsignedInt(opCodes[0]) >> 4];
        int data = 65025 * opCodes[1] + 255 * opCodes[2] + opCodes[3];
        reg.step();
        return new OpCode(opcode, cond, data);
    }

    public void execute(OpCode opcode) {
        if (opcode.instruction == Instructions.B)
            if (reg.canExecute(opcode.condition))
                branch(opcode.data);
    }
}
