package com.kbkapps.playgba.cpu;

public class Registers {
    public static final int SYSTEM_OR_USER = 0;
    public static final int SUPERVISOR = 1;
    public static final int ABORT = 2;
    public static final int INTERRUPT_REQUEST = 3;
    public static final int UNDEFINED = 4;
    //Registers 0-12
    private final int[] unbankedReg = new int[13];
    //Registers 13(SP) and 14(LR)
    private final int[] bankedReg = new int[10];
    private final int[] savedStatusReg = new int[5];
    private int mode;
    //PC
    private int reg15;
    private int currentStatusReg;

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
        if (mode.contains("sys")) {
            this.mode = SYSTEM_OR_USER;
        } else if (mode.contains("svc")) {
            this.mode = SUPERVISOR;
        } else if (mode.contains("abt")) {
            this.mode = ABORT;
        } else if (mode.contains("irq")) {
            this.mode = INTERRUPT_REQUEST;
        } else if (mode.contains("und")) {
            this.mode = UNDEFINED;
        } else throw new IndexOutOfBoundsException();
    }

    public int getPSR(char type) {
        if (type == 'C')
            return currentStatusReg;
        if (type == 'S')
            return savedStatusReg[mode];
        throw new IndexOutOfBoundsException();
    }

    public void setPSR(char type, int data) {
        if (type == 'C')
            currentStatusReg = data;
        else if (type == 'S')
            savedStatusReg[mode] = data;
        else throw new IndexOutOfBoundsException();
    }
}