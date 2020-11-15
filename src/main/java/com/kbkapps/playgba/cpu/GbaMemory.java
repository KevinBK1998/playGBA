package com.kbkapps.playgba.cpu;

import java.io.FileInputStream;
import java.io.IOException;

public class GbaMemory {
    public static final int IO_REG_OFFSET = 0x4000000;
    private static final int BIOS_SIZE = 0x0004000;
    private static final int SLOW_WORK_RAM_OFFSET = 0x2000000;
    private static final int SLOW_WORK_RAM_END = 0x2040000;
    private static final int WORK_RAM_OFFSET = 0x3000000;
    private static final int WORK_RAM_END = 0x3008000;
    byte[] bios;
    byte[] slowWorkRam = new byte[0x40000];   //256KB
    byte[] workRam = new byte[0x8000];   //32KB
    IORegisters registers = new IORegisters();

    public GbaMemory() {
        try {
            FileInputStream in = new FileInputStream("src/main/resources/bios.bin");
            bios = new byte[in.available()];
            if (in.read(bios) < BIOS_SIZE)
                System.out.println("ERROR: UNABLE TO READ COMPLETELY");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void write8(int address, byte data) throws WriteDeniedException {
        if (address < BIOS_SIZE)
            throw new WriteDeniedException(address);
        else if (address < SLOW_WORK_RAM_END)
            slowWorkRam[address - SLOW_WORK_RAM_OFFSET] = data;
        else if (address < WORK_RAM_END)
            workRam[address - WORK_RAM_OFFSET] = data;
        else
            registers.write8(address - IO_REG_OFFSET, data);
    }

    public byte read8(int address) {
        if (address < BIOS_SIZE)
            return bios[address];
        else if (address < SLOW_WORK_RAM_END)
            return slowWorkRam[address - SLOW_WORK_RAM_OFFSET];
        else if (address < WORK_RAM_END)
            return workRam[address - WORK_RAM_OFFSET];
        return registers.read8(address - IO_REG_OFFSET);
    }

    public void write32(int address, int data) throws WriteDeniedException {
        for (int i = address; i < address + 4; i++) {
            write8(i, (byte) data);
            data = data >> 8;
        }
    }

    public int read32(int address) {
        int completeWord = 0;
        for (int i = address; i < address + 4; i++) {
            byte byteValue = read8(i);
            completeWord += Byte.toUnsignedInt(byteValue) << (8 * i);
        }
        return completeWord;
    }
}
