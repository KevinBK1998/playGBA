package com.kbkapps.playgba.cpu;

public enum Flags {
    EQ, NE, CS, CC, MI, PL, VS, VC, HI, LS, GE, LT, GT, LE, AL;

    @Override
    public String toString() {
        switch (this) {
            case EQ:
                return "equal";
            case NE:
                return "not equal";
            case CS:
                return "carry set";
            case CC:
                return "carry cleared";
            case MI:
                return "minus";
            case PL:
                return "plus";
            case VS:
                return "signed overflow";
            case VC:
                return "signed no overflow";
            case HI:
                return "unsigned higher";
            case LS:
                return "unsigned lower or same";
            case GE:
                return "signed greater or equal";
            case LT:
                return "signed less than";
            case GT:
                return "signed greater than";
            case LE:
                return "signed less or equal";
            case AL:
                return "always";
            default:
                throw new IndexOutOfBoundsException();
        }
    }
}
