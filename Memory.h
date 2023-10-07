#ifndef MEMORY_H
#define MEMORY_H

const char* BIOS_FILE_NAME = "gba_bios.bin";
const int BIOS_FILE_SIZE = 0x4000;
const int WRAM_FILE_SIZE = 0x8000;
const int WRAM_OFFSET = 0x3000000;
const int WRAM_END = 0x3008000;
const int IO_REG_OFFSET = 0x4000000;
const int IO_REG_END = 0x4000400;
const int ROM_OFFSET = 0x8000000;
const int ROM_END = 0xE010000;

#endif
