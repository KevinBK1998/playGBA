package com.kbkapps.playgba.cpu;

public class Registers {
    private final int[] unbankedReg = new int[13];

    int getReg(int index) {
        return unbankedReg[index];
    }

    void setReg(int data, int index) {
        unbankedReg[index] = data;
    }
}