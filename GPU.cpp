#include <iostream>
#include "CommonDS.h"
#include "FailureCodes.h"
#include "GPU.h"

void GPU::storeHalfWord(int address, uint16_t* var, uint8_t data){
    if (address){
        *(var) &= 0xFF;
        *(var) |= (data<<8);
        if(DEBUG_LOGS)
            status();
    }
    else {
        *(var) &= 0xFF00;
        *(var) |= data;
    }
}

void GPU::storeWord(int address, uint32_t* var, uint8_t data){
    if (address == 0){
        *(var) &= 0xFFFFFF00;
        *(var) |= data;
    }
    else if (address == 1){
        *(var) &= 0xFFFF00FF;
        *(var) |= (data<<8);
    }
    else if (address == 2){
        *(var) &= 0xFF00FFFF;
        *(var) |= (data<<16);
    }
    else {
        *(var) &= 0xFFFFFF;
        *(var) |= (data<<24);
        if(DEBUG_LOGS)
            status();
    }
}

void GPU::write8(uint8_t address, uint8_t data){
    uint16_t* pointer;
    uint32_t* wordPointer;
    int actualAddress;
    bool isWord=false;
    switch (address & 0xFE)
    {
    case DISPCNT ... DISPCNT+1:
        pointer = &dispCnt;
        break;
    case DISPSTAT ... DISPSTAT+1:
        pointer = &dispStat;
        break;
    case VCOUNT ... VCOUNT+1:
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
    case BG2X ... BG2X+3:
        isWord = true;
        wordPointer = &bg2x;
        break;
    case BG2Y ... BG2Y+3:
        isWord = true;
        wordPointer = &bg2y;
        break;
    case BG3PAR_OFFSET ... BG3PAR_OFFSET_END:
        actualAddress = (address>>1) & 0b11;
        pointer = &bg3Parameters[actualAddress];
        break;
    case BG3X ... BG3X+3:
        isWord = true;
        wordPointer = &bg3x;
        break;
    case BG3Y ... BG3Y+3:
        isWord = true;
        wordPointer = &bg3y;
        break;
    case WIN0H ... WIN0H+1:
        pointer = &win0x;
        break;
    case WIN1H ... WIN1H+1:
        pointer = &win1x;
        break;
    case WIN0V ... WIN0V+1:
        pointer = &win0y;
        break;
    case WIN1V ... WIN1V+1:
        pointer = &win1y;
        break;
    case WININ ... WININ+1:
        pointer = &winIn;
        break;
    case WINOUT ... WINOUT+1:
        pointer = &winOut;
        break;
    case MOSAIC ... MOSAIC+1:
        pointer = &mosaic;
        break;
    case MOSAIC+2 ... MOSAIC+3:
        cout << "W: GPU Unused Memory: "<<unsigned(address)<<endl;
        return;
    case BLDCNT ... BLDCNT+1:
        pointer = &sfxCnt;
        break;
    case BLDALPHA ... BLDALPHA+1:
        pointer = &alphaCoeff;
        break;
    case BLDY ... BLDY+1:
        pointer = &brightCoeff;
        break;
    case BLDY+2 ... BLDY+11:
        cout << "W: GPU Unused Memory: "<<unsigned(address)<<endl;
        return;
    default:
        cout << "W: GPU Undefined Memory: "<<unsigned(address)<<endl;
        exit(FAILED_DMA);
    }
    if(isWord)
        storeWord(address&0b11, wordPointer, data);
    else
        storeHalfWord(address&1, pointer, data);
}

void GPU::status(){
    cout<<"\nGPU:"<<endl;
    cout<<"DISPCNT: "<<dispCnt<<"\tDISPSTAT: "<<dispStat<<"\tVCOUNT: "<<vCount<<endl;
    cout<<"BG0CNT: "<<bgCnt[0]<<"\tBG1CNT: "<<bgCnt[1]<<"\tBG2CNT: "<<bgCnt[2]<<"\tBG3CNT: "<<bgCnt[3]<<endl;
    cout<<"BG0HOFS: "<<bgOffsetX[0]<<"\tBG0HOFS: "<<bgOffsetX[1]<<"\tBG0HOFS: "<<bgOffsetX[2]<<"\tBG0HOFS: "<<bgOffsetX[3]<<endl;
    cout<<"BG0VOFS: "<<bgOffsetY[0]<<"\tBG0VOFS: "<<bgOffsetY[1]<<"\tBG0VOFS: "<<bgOffsetY[2]<<"\tBG0VOFS: "<<bgOffsetY[3]<<endl;
    cout<<"BG2PA: "<<bg2Parameters[0]<<"\tBG2PB: "<<bg2Parameters[1]<<"\tBG2PC: "<<bg2Parameters[2]<<"\tBG2PD: "<<bg2Parameters[3]<<"\tBG2X: "<<bg2x<<"\tBG2Y: "<<bg2y<<endl;
    cout<<"BG3PA: "<<bg3Parameters[0]<<"\tBG3PB: "<<bg3Parameters[1]<<"\tBG3PC: "<<bg3Parameters[2]<<"\tBG3PD: "<<bg3Parameters[3]<<"\tBG3X: "<<bg3x<<"\tBG3Y: "<<bg3y<<endl;
    cout<<"WIN0H: "<<win0x<<"\tWIN1H: "<<win1x<<"\tWIN0V: "<<win0y<<"\tWIN1V: "<<win1y<<"\tWININ: "<<winIn<<"\tWINOUT: "<<winOut<<endl;
    cout<<"MOSAIC: "<<mosaic<<"\tBLDCNT: "<<sfxCnt<<"\tBLDALPHA: "<<alphaCoeff<<"\tBLDY: "<<brightCoeff<<endl<<endl;
}