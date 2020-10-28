package com.kbkapps.playgba.cpu;

import com.kbkapps.playgba.cpu.constants.Flags;

import static com.kbkapps.playgba.cpu.constants.Flags.*;

public class Registers {
    public static final int SYSTEM_OR_USER = 0;
    public static final int SUPERVISOR = 1;
    public static final int ABORT = 2;
    public static final int INTERRUPT_REQUEST = 3;
    public static final int UNDEFINED = 4;
    static final char CPSR = 'C';
    private static final char SPSR = 'S';
    //Registers 0-12
    private final int[] unbankedReg = new int[13];
    //Registers 13(SP) and 14(LR)
    private final int[] bankedReg = new int[10];
    private final int[] savedStatusReg = new int[5];
    private int mode;
    //PC
    private int reg15;
    private int currentStatusReg;

    public Registers() {
    }

    public Registers(int[] unbankedReg, int[] bankedReg, int[] savedStatusReg, int mode, int r15, int cpsr) {
        System.arraycopy(unbankedReg, 0, this.unbankedReg, 0, 13);
        System.arraycopy(bankedReg, 0, this.bankedReg, 0, 10);
        System.arraycopy(savedStatusReg, 0, this.savedStatusReg, 0, 5);
        this.mode = mode;
        reg15 = r15;
        currentStatusReg = cpsr;
    }

    int getReg(int index) {
        if (index < 0)
            throw new IndexOutOfBoundsException();
        if (index < 13)
            return unbankedReg[index];
        if (index < 15)
            return bankedReg[getBankedIndex(index)];
        if (index == 15)
            return reg15;
        throw new IndexOutOfBoundsException();
    }

    private int getBankedIndex(int index) {
        index -= 13;
        return index * 5 + mode;
    }

    void setReg(int index, int data) {
        if (index < 0)
            throw new IndexOutOfBoundsException();
        if (index < 13)
            unbankedReg[index] = data;
        else if (index < 15)
            bankedReg[getBankedIndex(index)] = data;
        else if (index == 15)
            reg15 = data;
        else
            throw new IndexOutOfBoundsException();
    }

    public void setMode(String mode) {
        switch (mode) {
            case "sys":
                this.mode = SYSTEM_OR_USER;
                break;
            case "svc":
                this.mode = SUPERVISOR;
                break;
            case "abt":
                this.mode = ABORT;
                break;
            case "irq":
                this.mode = INTERRUPT_REQUEST;
                break;
            case "und":
                this.mode = UNDEFINED;
                break;
            default:
                throw new IndexOutOfBoundsException();
        }
    }

    public int getPSR(char type) {
        if (type == CPSR)
            return currentStatusReg;
        if (type == SPSR)
            return savedStatusReg[mode];
        throw new IndexOutOfBoundsException();
    }

    public void setPSR(char type, int data) {
        if (type == CPSR)
            currentStatusReg = data;
        else if (type == SPSR)
            savedStatusReg[mode] = data;
        else throw new IndexOutOfBoundsException();
    }

    public boolean canExecute(Flags cond) {
        //NegativeZeroCarryOverflow(V)----  --------  --------  IrqFiqThumbMMMMMode
        if (cond == AL) return true;
        else if (cond == MI) return currentStatusReg < 0;
        else if (cond == PL) return currentStatusReg >= 0;
        else if (cond == EQ) return (currentStatusReg & 0x40_00_00_00) != 0;
        else if (cond == NE) return (currentStatusReg & 0x40_00_00_00) == 0;
        else if (cond == CS) return (currentStatusReg & 0x20_00_00_00) != 0;
        else if (cond == CC) return (currentStatusReg & 0x20_00_00_00) == 0;
        else if (cond == VS) return (currentStatusReg & 0x10_00_00_00) != 0;
        else if (cond == VC) return (currentStatusReg & 0x10_00_00_00) == 0;
        else if (cond == HI) return canExecute(CS) && canExecute(NE);
        else if (cond == LS) return canExecute(CC) || canExecute(EQ);
        else if (cond == GE) return canExecute(MI) == canExecute(VS);
        else if (cond == LT) return canExecute(MI) != canExecute(VS);
        else if (cond == GT) return canExecute(NE) && canExecute(GE);
        else if (cond == LE) return canExecute(EQ) || canExecute(LT);
        else throw new IndexOutOfBoundsException(cond.name());
    }

    public void step() {
        reg15 += 4;
    }

    @Override
    public boolean equals(Object obj) {
        if (obj instanceof Registers)
            return this.toString().equalsIgnoreCase(obj.toString());
        return false;
    }

    @Override
    public String toString() {
        StringBuilder regState = new StringBuilder();
        for (int i = 0; i < 13; i++)
            regState.append(Integer.toUnsignedString(unbankedReg[i], 16)).append(" ");
        for (int i = 0; i < 10; i++)
            regState.append(Integer.toUnsignedString(bankedReg[i], 16)).append(" ");
        for (int i = 0; i < 5; i++)
            regState.append(Integer.toUnsignedString(savedStatusReg[i], 16)).append(" ");
        regState.append(Integer.toUnsignedString(mode, 16)).append(" ")
                .append(Integer.toUnsignedString(reg15, 16)).append(" ")
                .append(Integer.toUnsignedString(currentStatusReg, 16));

        return regState.toString();
    }
}