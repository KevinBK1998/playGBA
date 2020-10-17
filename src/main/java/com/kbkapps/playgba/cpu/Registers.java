package com.kbkapps.playgba.cpu;

public class Registers {
    private final int[] unbankedReg = new int[13];
    private int reg15;

    int getReg(int index) {
        if (index < 0)
            throw new IndexOutOfBoundsException();
        if (index < 13)
            return unbankedReg[index];
        if (index == 15)
            return reg15;
        return 0;
    }

    void setReg(int data, int index) {
        if (index < 0)
            throw new IndexOutOfBoundsException();
        if (index < 13)
            unbankedReg[index] = data;
        if (index == 15)
            reg15 = data;
    }
}