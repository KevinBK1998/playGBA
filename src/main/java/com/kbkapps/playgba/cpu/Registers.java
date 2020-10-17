package com.kbkapps.playgba.cpu;

public class Registers {
    private final int[] unbankedReg = new int[13];
    private final int[] bankedReg = new int[10];
    private int reg15;

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
        return index * 5;
    }

    void setReg(int data, int index) {
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
}