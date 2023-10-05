#include <iostream>
#include <fstream>
#include "Memory.h"

using namespace std;

class Memory
{
private:
    int bios[BIOS_FILE_SIZE];
    void loadBios(const char *fileName){
        char c;
        ifstream fin(fileName);
        if (!fin)
        {
            cout << "Error Opening BIOS\n";
            exit(-1);
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
    ~Memory();
    uint8_t read8(int address) {
        cout << "Address: 0x"<<address<<endl;
        if (address < BIOS_FILE_SIZE)
            return bios[address];
        // else if (address >= SLOW_WORK_RAM_OFFSET && address < SLOW_WORK_RAM_END)
        //     return slowWorkRam[address - SLOW_WORK_RAM_OFFSET];
        // else if (address >= WORK_RAM_OFFSET && address < WORK_RAM_END)
        //     return workRam[address - WORK_RAM_OFFSET];
        // else if (address >= IO_REG_OFFSET && address < IO_REG_END)
        //     return registers.read8(address - IO_REG_OFFSET);
        // else if (address >= VRAM_OFFSET)
        //     throw new IndexOutOfBoundsException("R: Unknown Memory: 0x" + Integer.toHexString(address));
        cout << "R: Unused Memory: 0x"<<address<<endl;
        return 0;
    }
    int read32(int address) {
        cout << "Address: 0x"<<address<<endl;
        int completeWord = 0;
        for (int i = address; i < address + 4; i++) {
            uint8_t byteValue = read8(i);
            completeWord += ((int) byteValue) << (8 * i);
        }
        return completeWord;
    }
};

Memory::~Memory()
{
}
