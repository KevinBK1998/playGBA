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
    int actualAddress;
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
        actualAddress = (address>>1) & 0b11;
        pointer = &bgCnt[actualAddress];
        break;
    case BGHW_OFFSET ... BGHW_OFFSET_END:
        actualAddress = (address>>1) & 0b111;
        pointer = (actualAddress&1)? &bgOffsetY[actualAddress>>1]: &bgOffsetX[actualAddress>>1];
        break;
    case BG2PAR_OFFSET ... BG2PAR_OFFSET_END:
        actualAddress = (address>>1) & 0b11;
        pointer = &bg2Parameters[actualAddress];
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
    cout<<"BG0CNT: "<<bgCnt[0]<<"\tBG1CNT: "<<bgCnt[1]<<"\tBG2CNT: "<<bgCnt[2]<<"\tBG3CNT: "<<bgCnt[3]<<endl;
    cout<<"BG0HOFS: "<<bgOffsetX[0]<<"\tBG0HOFS: "<<bgOffsetX[1]<<"\tBG0HOFS: "<<bgOffsetX[2]<<"\tBG0HOFS: "<<bgOffsetX[3]<<endl;
    cout<<"BG0VOFS: "<<bgOffsetY[0]<<"\tBG0VOFS: "<<bgOffsetY[1]<<"\tBG0VOFS: "<<bgOffsetY[2]<<"\tBG0VOFS: "<<bgOffsetY[3]<<endl;
    cout<<"BG2PA: "<<bg2Parameters[0]<<"\tBG2PB: "<<bg2Parameters[1]<<"\tBG2PC: "<<bg2Parameters[2]<<"\tBG2PD: "<<bg2Parameters[3]<<endl<<endl;
}
