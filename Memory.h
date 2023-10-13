#ifndef MEMORY_H
#define MEMORY_H

#include "GPU.h"
#include "APU.h"
#include "IORegisters.cpp"

const char* BIOS_FILE_NAME = "gba_bios.bin";
const int BIOS_FILE_SIZE = 0x4000;
const int WRAM_FILE_SIZE = 0x8000;
const int SLOW_WRAM_FILE_SIZE = 0x40000;
const int SLOW_WRAM_OFFSET = 0x2000000;
const int WRAM_OFFSET = 0x3000000;
const int IO_REG_OFFSET = 0x4000000;
const int GPU_REG_END = 0x4000060;
const int APU_REG_END = 0x40000B0;
const int IO_REG_END = 0x4000400;
const int UNKNOWN_BIOS_FLAG = 0x4000410;
const int ROM_OFFSET = 0x8000000;
const int ROM_END = 0xE010000;

class Memory
{
private:
    uint8_t bios[BIOS_FILE_SIZE];
    uint8_t slowRam[SLOW_WRAM_FILE_SIZE];
    uint8_t wram[WRAM_FILE_SIZE];
    uint8_t rom[200];
    IORegisters registers;
    GPU gpu;
    APU apu;
    void loadBios(const char *);
public:
    Memory(GPU);
    Memory(GPU, char *);
    uint8_t read8(uint32_t);
    void write8(uint32_t, uint8_t);
    uint16_t read16(uint32_t);
    void write16(uint32_t, uint16_t);
    uint32_t read32(uint32_t);
    void write32(uint32_t, uint32_t);
    void dump();
};

#endif
