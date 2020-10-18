package com.kbkapps.playgba.cpu;

import static com.kbkapps.playgba.cpu.Flags.AL;
import static com.kbkapps.playgba.cpu.Flags.EQ;

public class ArmV3Cpu {
    public static final byte HS = 0x2;
    public static final byte LO = 0x3;
    public static final int PC = 15;
    Registers reg;

    public ArmV3Cpu(Registers registers) {
        reg = registers;
    }

    public void runOpcode(String opCode, String condition, int label) {
        Flags cond = null;
        if (condition.equals("AL"))
            cond = AL;
        else if (condition.equals("EQ"))
            cond = EQ;
        if (opCode.equals("B"))
            if (reg.canExecute(cond))
                branch(label);
    }

    private void branch(int label) {
        reg.setReg(PC, (int) (Integer.toUnsignedLong(reg.getReg(PC)) + label * 4));
    }
}
