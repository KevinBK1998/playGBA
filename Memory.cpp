#include <iostream>
#include <fstream>
#include "CommonDS.h"
#include "Memory.h"

using namespace std;

void Memory::loadBios(const char *fileName){
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
    DEBUG_OUT << "Loaded Bios:" << fileName << endl;
}

Memory::Memory(){
    loadBios(BIOS_FILE_NAME);
}

Memory::Memory(char *fileName):Memory::Memory(){
    char c;
    ifstream fin(fileName);
    if (!fin)
    {
        cout << "Error Opening ROM\n";
        exit(FAILED_TO_LOAD_ROM);
    }
    int i = 0;
    while (i < BIOS_FILE_SIZE && fin)
    {
        fin.get(c);
        rom[i++] = c;
    }
    DEBUG_OUT << "Loaded Rom:" << fileName << endl;
}

uint8_t Memory::read8(uint32_t address) {
    // DEBUG_OUT << "Address: "<<address<<endl;
    if (address < BIOS_FILE_SIZE)
        return bios[address];
    // else if (address >= SLOW_WORK_RAM_OFFSET && address < SLOW_WORK_RAM_END)
    //     return slowWorkRam[address - SLOW_WORK_RAM_OFFSET];
    else if (address >= WRAM_OFFSET && address < WRAM_END)
        return wram[address - WRAM_OFFSET];
    else if (address >= IO_REG_OFFSET && address < IO_REG_END)
    {
        if (address>GPU_REG_END && address < APU_REG_END)
            return apu.read8(address);
        else
            return registers.read8(address - IO_REG_OFFSET);
    }
    else if (address >= ROM_OFFSET && address < ROM_END)
        return rom[address - ROM_OFFSET];
    // else if (address >= VRAM_OFFSET)
    //     throw new IndexOutOfBoundsException("R: Unknown Memory: 0x" + Integer.toHexString(address));
    cout << "R: Undefined Memory: "<<address<<endl;
    exit(FAILED_DMA);
    return 0;
}

uint16_t Memory::read16(uint32_t address) {
    // DEBUG_OUT << "Address: 0x"<<address<<endl;
    int completeHalfWord = 0;
    for (int i = 0; i < HALFWORD_SIZE; i++) {
        uint8_t byteValue = read8(address+i);
        completeHalfWord += ((int) byteValue) << (8 * i);
    }
    return completeHalfWord;
}

uint32_t Memory::read32(uint32_t address) {
    // DEBUG_OUT << "Address: 0x"<<address<<endl;
    int completeWord = 0;
    for (int i = 0; i < WORD_SIZE; i++) {
        uint8_t byteValue = read8(address+i);
        completeWord += ((int) byteValue) << (8 * i);
    }
    return completeWord;
}

void Memory::write8(uint32_t address, uint8_t data) {
    // DEBUG_OUT << "Address: "<<address<<", Data: "<<unsigned(data)<<endl;
    if (address >= WRAM_OFFSET && address < WRAM_END)
        wram[address - WRAM_OFFSET] = data;
    else if(address >= 0x3fffe00 && address < IO_REG_OFFSET){
        DEBUG_OUT << "W: Unused Memory: "<<address<<", Data: "<<unsigned(data)<<endl;
        return;
    }
    else if (address >= IO_REG_OFFSET && address < IO_REG_END){
        if (address < GPU_REG_END)
            gpu.write8(address, data);
        else if (address < APU_REG_END)
            apu.write8(address, data);
        else
            registers.write8(address - IO_REG_OFFSET, data);
    }
    else if (address == UNKNOWN_BIOS_FLAG)
            DEBUG_OUT << "W: UNKNOWN_BIOS_FLAG: "<<unsigned(data)<<endl;
    // else if (address >= ROM_OFFSET && address < ROM_END)
    //     rom[address - ROM_OFFSET] = data;
    else{
        cout << "W: Undefined Memory: "<<address<<", Data: "<<unsigned(data)<<endl;
        exit(FAILED_DMA);
    }
    // if (address < BIOS_SIZE)
    //     throw new WriteDeniedException(address);
    // else if (address >= SLOW_WORK_RAM_OFFSET && address < SLOW_WORK_RAM_END)
    //     slowWorkRam[address - SLOW_WORK_RAM_OFFSET] = data;
    // else if (address >= VRAM_OFFSET)
    //     throw new IndexOutOfBoundsException("W: Unknown Memory: 0x" + Integer.toHexString(address) + ", set to 0x" + Integer.toHexString(data));
    // else
    //     System.out.println("Unused Memory: 0x" + Integer.toHexString(address) + ", set to 0x" + Integer.toHexString(data));
}

void Memory::write16(uint32_t address, uint16_t data) {
    // DEBUG_OUT << "Address: "<<address<<", Data: "<<data<<endl;
    for (int i = 0; i < HALFWORD_SIZE; i++) {
        write8(address + i, data);
        data = data >> 8;
    }
}

void Memory::write32(uint32_t address, uint32_t data) {
    // DEBUG_OUT << "Address: "<<address<<", Data: "<<data<<endl;
    for (int i = 0; i < WORD_SIZE; i++) {
        write8(address + i, data);
        data = data >> 8;
    }
}
