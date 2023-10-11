#ifndef GPU_H
#define GPU_H

#include <stdint.h>
using namespace std;

const int DISPCNT = 0;
const int DISPSTAT = 4;
const int VCOUNT = 6;
const int BGCNT_OFFSET = 8;
const int BGCNT_END = 0xE;
const int BGHW_OFFSET = 0x10;
const int BGHW_OFFSET_END = 0x1E;
const int BG2PAR_OFFSET = 0x20;
const int BG2PAR_OFFSET_END = 0x26;

class GPU{
    uint16_t dispCnt;
    uint16_t dispStat;
    uint16_t vCount;
    uint16_t bgCnt[4];
    uint16_t bgOffsetX[4];
    uint16_t bgOffsetY[4];
    uint16_t bg2Parameters[4];
    void storeHalfWord(int, uint16_t*, uint8_t);
public:
    void write8(uint8_t, uint8_t);
    void status();
};

#endif
