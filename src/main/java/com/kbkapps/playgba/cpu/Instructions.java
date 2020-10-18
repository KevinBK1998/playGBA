package com.kbkapps.playgba.cpu;

public class Instructions {
    public static final int PC = 15;
    Registers reg;

    public Instructions(Registers registers) {
        reg = registers;
    }

    public void setOpcode(String b, int label) {
        reg.setReg(PC, (int) (Integer.toUnsignedLong(reg.getReg(PC)) + label * 4));
    }
}
