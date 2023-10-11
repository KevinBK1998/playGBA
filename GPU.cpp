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
    uint16_t* pointer;
    switch (address & 0xFE)
    {
    case DISPCNT:
        pointer = &dispCnt;
        break;
    case DISPSTAT:
        pointer = &dispStat;
        break;
    case VCOUNT:
        pointer = &vCount;
        break;
    case BGCNT_OFFSET ... BGCNT_END:
        pointer = &bgCnt[(address>>1) & 0b11];
        break;
    default:
        cout << "W: GPU Undefined Memory: "<<unsigned(address)<<endl;
        exit(FAILED_DMA);
    }
    storeHalfWord(address&1, pointer, data);
    if(address&1)
        status();
}

void GPU::status(){
    cout<<"\nGPU:"<<endl;
    cout<<"DISPCNT: "<<dispCnt<<"\tDISPSTAT: "<<dispStat<<"\tVCOUNT: "<<vCount<<endl;
    cout<<"BG0CNT: "<<bgCnt[0]<<"\tBG1CNT: "<<bgCnt[1]<<"\tBG2CNT: "<<bgCnt[2]<<"\tBG3CNT: "<<bgCnt[3]<<endl<<endl;
}
