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
    public static final int IO_REG_END = 0x4000400;
    public static final int VRAM_OFFSET = 0x5000000;
    byte[] bios;
    byte[] slowWorkRam = new byte[0x40000];   //256KB
    byte[] workRam = new byte[0x8000];   //32KB
    IORegisters registers = new IORegisters();

    public GbaMemory() {
        try {
            FileInputStream in = new FileInputStream("src/main/resources/bios.bin");
            bios = new byte[in.available()];
            if (in.read(bios) < BIOS_SIZE)
                System.out.println("ERROR: UNABLE TO READ BIOS COMPLETELY");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void write8(int address, byte data) throws WriteDeniedException {
//        System.out.printf("Address: 0x%x, Data: 0x%x\n",address,data);
        if (address < BIOS_SIZE)
            throw new WriteDeniedException(address);
        else if (address >= SLOW_WORK_RAM_OFFSET && address < SLOW_WORK_RAM_END)
            slowWorkRam[address - SLOW_WORK_RAM_OFFSET] = data;
        else if (address >= WORK_RAM_OFFSET && address < WORK_RAM_END)
            workRam[address - WORK_RAM_OFFSET] = data;
        else if (address >= IO_REG_OFFSET && address < IO_REG_END)
            registers.write8(address - IO_REG_OFFSET, data);
        else if (address >= VRAM_OFFSET)
            throw new IndexOutOfBoundsException("W: Unknown Memory: 0x" + Integer.toHexString(address) + ", set to 0x" + Integer.toHexString(data));
        else
            System.out.println("Unused Memory: 0x" + Integer.toHexString(address) + ", set to 0x" + Integer.toHexString(data));
    }

    public byte read8(int address) {
//        System.out.printf("Address: 0x%x\n",address);
        if (address < BIOS_SIZE)
            return bios[address];
        else if (address >= SLOW_WORK_RAM_OFFSET && address < SLOW_WORK_RAM_END)
            return slowWorkRam[address - SLOW_WORK_RAM_OFFSET];
        else if (address >= WORK_RAM_OFFSET && address < WORK_RAM_END)
            return workRam[address - WORK_RAM_OFFSET];
        else if (address >= IO_REG_OFFSET && address < IO_REG_END)
            return registers.read8(address - IO_REG_OFFSET);
        else if (address >= VRAM_OFFSET)
            throw new IndexOutOfBoundsException("R: Unknown Memory: 0x" + Integer.toHexString(address));
        System.out.println("R: Unused Memory: 0x" + Integer.toHexString(address));
        return 0;
    }

    public void write32(int address, int data) throws WriteDeniedException {
//        System.out.printf("Address: 0x%x, Data: 0x%x\n", address, data);
        for (int i = address; i < address + 4; i++) {
            write8(i, (byte) data);
            data = data >> 8;
        }
    }

    public int read32(int address) {
//        System.out.printf("Address: 0x%x\n", address);
        int completeWord = 0;
        for (int i = address; i < address + 4; i++) {
            byte byteValue = read8(i);
            completeWord += Byte.toUnsignedInt(byteValue) << (8 * i);
        }
        return completeWord;
    }
}
