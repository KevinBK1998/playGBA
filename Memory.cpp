#include <iostream>
#include <fstream>
#include "Memory.h"
#include "IORegisters.cpp"

using namespace std;

class Memory
{
private:
    int bios[BIOS_FILE_SIZE];
    int wram[WRAM_FILE_SIZE];
    IORegisters registers;
    void loadBios(const char *fileName){
        char c;
        ifstream fin(fileName);
        if (!fin)
        {
            cout << "Error Opening BIOS\n";
            exit(FAILED_TO_LOAD_BIOS);
        }
        int i = 0;
        while (i < BIOS_FILE_SIZE && fin)
        {
            fin.get(c);
            bios[i++] = c;
        }
        cout << "Loaded Bios:" << fileName << endl;
    }
public:
    Memory(/* args */){
        loadBios(BIOS_FILE_NAME);
    }
    ~Memory(){}

    uint8_t read8(int address) {
        // cout << "Address: "<<address<<endl;
        if (address < BIOS_FILE_SIZE)
            return bios[address];
        // else if (address >= SLOW_WORK_RAM_OFFSET && address < SLOW_WORK_RAM_END)
        //     return slowWorkRam[address - SLOW_WORK_RAM_OFFSET];
        else if (address >= WRAM_OFFSET && address < WRAM_END)
            return wram[address - WRAM_OFFSET];
        else if (address >= IO_REG_OFFSET && address < IO_REG_END)
            return registers.read8(address - IO_REG_OFFSET);
        // else if (address >= VRAM_OFFSET)
        //     throw new IndexOutOfBoundsException("R: Unknown Memory: 0x" + Integer.toHexString(address));
        cout << "R: Unused Memory: "<<address<<endl;
        exit(FAILED_DMA);
        return 0;
    }

    void write8(int address, uint8_t data) {
        // cout << "Address: "<<address<<", Data: "<<unsigned(data)<<endl;
        if (address >= WRAM_OFFSET && address < WRAM_END)
            wram[address - WRAM_OFFSET] = data;
        else if (address >= IO_REG_OFFSET && address < IO_REG_END)
            registers.write8(address - IO_REG_OFFSET, data);
        else{
            cout << "W: Unused Memory: "<<address<<", Data: "<<unsigned(data)<<endl;
            exit(FAILED_DMA);
        }
        // if (address < BIOS_SIZE)
        //     throw new WriteDeniedException(address);
        // else if (address >= SLOW_WORK_RAM_OFFSET && address < SLOW_WORK_RAM_END)
        //     slowWorkRam[address - SLOW_WORK_RAM_OFFSET] = data;

        // else if (address >= IO_REG_OFFSET && address < IO_REG_END)
        //     registers.write8(address - IO_REG_OFFSET, data);
        // else if (address >= VRAM_OFFSET)
        //     throw new IndexOutOfBoundsException("W: Unknown Memory: 0x" + Integer.toHexString(address) + ", set to 0x" + Integer.toHexString(data));
        // else
        //     System.out.println("Unused Memory: 0x" + Integer.toHexString(address) + ", set to 0x" + Integer.toHexString(data));
    }

    int read32(int address) {
        // cout << "Address: 0x"<<address<<endl;
        int completeWord = 0;
        for (int i = address; i < address + 4; i++) {
            uint8_t byteValue = read8(i);
            completeWord += ((int) byteValue) << (8 * i);
        }
        return completeWord;
    }

    void write32(int address, int data) {
        // cout << "Address: "<<address<<", Data: "<<data<<endl;
        for (int i = 0; i < 4; i++) {
            write8(address + i, data);
            data = data >> 8;
        }
    }
};
