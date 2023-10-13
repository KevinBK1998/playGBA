#ifndef APU_H
#define APU_H

#include <stdint.h>

const int SOUND1CNT_L = 0x60;
const int SOUND2CNT_H = 0x6C;
const int SOUND3CNT_L = 0x70;
const int SOUNDCNT_L = 0x80;
const int SOUNDCNT_H = 0x82;
const int SOUNDCNT_X = 0x84;
const int WAVE_RAM_OFFSET = 0x90;
const int FIFO_A = 0xA0;
const int FIFO_B = 0xA4;

class APU{
    uint16_t chan1sweep;
    uint16_t chan2frequency;
    uint16_t volumeCnt;
    uint16_t mixCnt;
    uint16_t apuCnt;
    uint16_t waveCnt;
    uint8_t waveRam[20];
    uint32_t fifoA;
    uint32_t fifoB;
    void storeHalfWord(int, uint16_t*, uint8_t);
    void storeWord(int, uint32_t*, uint8_t);
public:
    uint8_t read8(uint8_t);
    void write8(uint8_t, uint8_t);
    void status();
};

#endif
