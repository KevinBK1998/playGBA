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
        Flags cond;
        switch (condition) {
            case "EQ":
                cond = EQ;
                break;
            case "NE":
                cond = NE;
                break;
            case "CS":
                cond = CS;
                break;
            case "HS":
                cond = HS;
                break;
            case "CC":
                cond = CC;
                break;
            case "LO":
                cond = LO;
                break;
            case "MI":
                cond = MI;
                break;
            case "PL":
                cond = PL;
                break;
            case "VS":
                cond = VS;
                break;
            case "VC":
                cond = VC;
                break;
            case "HI":
                cond = HI;
                break;
            case "LS":
                cond = LS;
                break;
            case "GE":
                cond = GE;
                break;
            case "LT":
                cond = LT;
                break;
            case "GT":
                cond = GT;
                break;
            case "LE":
                cond = LE;
                break;
            case "AL":
                cond = AL;
                break;
            default:
                throw new IndexOutOfBoundsException(condition);
        }
        if (opCode.equals("B"))
            if (reg.canExecute(cond))
                branch(label);
    }

    private void branch(int label) {
        reg.setReg(PC, (int) (Integer.toUnsignedLong(reg.getReg(PC)) + label * 4));
    }
}
