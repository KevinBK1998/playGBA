package com.kbkapps.playgba.cpu;

public class ArmV3Cpu {
    public static final byte AL = 0xE;
    public static final int PC = 15;
    Registers reg;

    public ArmV3Cpu(Registers registers) {
        reg = registers;
    }

    public void runOpcode(String opCode, String condition, int label) {
        byte cond = 0;
        if (condition.equals("AL"))
            cond = AL;
        if (opCode.equals("B"))
            if (reg.isTrue(cond))
                branch(label);
    }

    private void branch(int label) {
        reg.setReg(PC, (int) (Integer.toUnsignedLong(reg.getReg(PC)) + label * 4));
    }
}
