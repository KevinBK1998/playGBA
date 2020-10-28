package com.kbkapps.playgba.cpu;

public class IORegisters {
    public static final int POST_BOOT_FLAG = 0x300;
    byte postBootFlag;

    public byte read8(int address) {
        if (address == POST_BOOT_FLAG)
            return postBootFlag;
        return 0;
    }

    public void write8(int address, byte data) {
        if (address == 0x300)
            postBootFlag = data;
    }
}
