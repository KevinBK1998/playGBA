#ifndef GPU_H
#define GPU_H

#include <stdint.h>
#include "CommonDS.h"

const int DISPCNT = 0;
const int GREENSWAP = 2;
const int DISPSTAT = 4;
const int VCOUNT = 6;
const int BGCNT_OFFSET = 8;
const int BGCNT_END = 0xE;
const int BGHW_OFFSET = 0x10;
const int BGHW_OFFSET_END = 0x1E;
const int BG2PAR_OFFSET = 0x20;
const int BG2PAR_OFFSET_END = 0x26;
const int BG2X = 0x28;
const int BG2Y = 0x2C;
const int BG3PAR_OFFSET = 0x30;
const int BG3PAR_OFFSET_END = 0x36;
const int BG3X = 0x38;
const int BG3Y = 0x3C;
const int WIN0H = 0x40;
const int WIN1H = 0x42;
const int WIN0V = 0x44;
const int WIN1V = 0x46;
const int WININ = 0x48;
const int WINOUT = 0x4A;
const int MOSAIC = 0x4C;
const int BLDCNT = 0x50;
const int BLDALPHA = 0x52;
const int BLDY = 0x54;

class GPU{
    uint16_t dispCnt;
    uint16_t greenSwap;
    uint16_t dispStat;
    uint16_t vCount;
    uint16_t bgCnt[4];
    uint16_t bgOffsetX[4];
    uint16_t bgOffsetY[4];
    uint16_t bg2Parameters[4];
    uint32_t bg2x;
    uint32_t bg2y;
    uint16_t bg3Parameters[4];
    uint32_t bg3x;
    uint32_t bg3y;
    uint16_t win0x;
    uint16_t win0y;
    uint16_t win1x;
    uint16_t win1y;
    uint16_t winIn;
    uint16_t winOut;
    uint16_t mosaic;
    uint16_t sfxCnt;
    uint16_t alphaCoeff;
    uint16_t brightCoeff;
    uint8_t vram[VRAM_FILE_SIZE];
    void storeHalfWord(int, uint16_t*, uint8_t);
    void storeWord(int, uint32_t*, uint8_t);
public:
    void write8(uint32_t, uint8_t);
    void status();
};

#endif
