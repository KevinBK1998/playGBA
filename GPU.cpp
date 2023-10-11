#include <iostream>
#include "FailureCodes.h"
#include "GPU.h"

void GPU::storeHalfWord(int address, uint16_t* var, uint8_t data){
    if (address){
        *(var) &= 0xFF;
        *(var) |= (data<<8);
    }
    else {
        *(var) &= 0xFF00;
        *(var) |= data;
    }
}

void GPU::write8(uint8_t address, uint8_t data){
    switch (address & 0xFE)
    {
    case DISPCNT:
        storeHalfWord(address&1, &dispCnt, data);
        break;
    default:
        cout << "W: GPU Undefined Memory: "<<unsigned(address)<<endl;
        exit(FAILED_DMA);
    }
    status();
}

void GPU::status(){
    cout<<"DISPCNT: "<<dispCnt<<endl;
}
