package com.kbkapps.playgba.cpu;

public class IORegisters {
    public static final int POST_BOOT_FLAG = 0x300;
    public static final int IME = 0x208;
    byte postBootFlag;
    boolean ime;

    public byte read8(int address) {
        if (address == POST_BOOT_FLAG)
            return postBootFlag;
        else if (address == IME)
            return (byte) (ime ? 1 : 0);
        throw new IndexOutOfBoundsException("R:Unknown Memory: 0x" + Integer.toHexString(address));
    }

    public void write8(int address, byte data) {
        if (address == 0x300)
            postBootFlag = data;
        else if (address == IME)
            ime = (data & 1) != 0;
        else
            throw new IndexOutOfBoundsException("W:Unknown Memory: 0x" + Integer.toHexString(address));
    }
}
