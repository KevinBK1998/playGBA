#ifndef GPU_H
#define GPU_H

#include <stdint.h>
using namespace std;

const int DISPCNT = 0;

class GPU{
    uint16_t dispCnt;
    void storeHalfWord(int, uint16_t*, uint8_t);
public:
    void write8(uint8_t, uint8_t);
    void status();
};

#endif
