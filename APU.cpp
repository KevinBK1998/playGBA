#include <iostream>
#include "APU.h"
#include "CommonDS.h"
#include "FailureCodes.h"

using namespace std;

void APU::storeHalfWord(int address, uint16_t* var, uint8_t data){
    if (address){
        *(var) &= 0xFF;
        *(var) |= (data<<8);
        status();
    }
    else {
        *(var) &= 0xFF00;
        *(var) |= data;
    }
}

void APU::storeWord(int address, uint32_t* var, uint8_t data){
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
        status();
    }
}

void APU::status(){
    DEBUG_OUT<<"\nAPU:"<<endl;
    DEBUG_OUT<<"SOUNDCNT_L: "<<volumeCnt<<"\tSOUNDCNT_H: "<<mixCnt<<"\tSOUNDCNT_X: "<<apuCnt<<endl;
    DEBUG_OUT<<"SOUND1CNT_L: "<<chan1sweep<<"\tSOUND2CNT_H: "<<chan2frequency<<"\tSOUND3CNT_L: "<<waveCnt<<endl;
    DEBUG_OUT<<"FIFOA: "<<fifoA<<"\tFIFOB: "<<fifoB<<endl<<endl;
}

uint8_t APU::read8(uint8_t address){
    switch (address & 0xFE){
    case SOUNDBIAS ... SOUNDBIAS+1:
        return biasCnt;
    case WAVE_RAM_OFFSET ... WAVE_RAM_OFFSET+0xF:
        return waveRam[address - WAVE_RAM_OFFSET];
    default:
        cout << "R: APU Undefined Memory: "<<unsigned(address)<<endl;
        exit(FAILED_DMA);
    }
}

void APU::write8(uint8_t address, uint8_t data){
    uint16_t* pointer;
    uint32_t* wordPointer;
    int actualAddress;
    bool isWord=false;
    switch (address & 0xFE)
    {
    case SOUND1CNT_L ... SOUND1CNT_L+1:
        pointer = &chan1sweep;
        break;
    case SOUND2CNT_H ... SOUND2CNT_H+1:
        pointer = &chan2frequency;
        break;
    case SOUND3CNT_L ... SOUND3CNT_L+1:
        pointer = &waveCnt;
        break;
    case SOUNDCNT_L ... SOUNDCNT_L+1:
        pointer = &volumeCnt;
        break;
    case SOUNDCNT_H ... SOUNDCNT_H+1:
        pointer = &mixCnt;
        break;
    case SOUNDCNT_X ... SOUNDCNT_X+1:
        pointer = &apuCnt;
        break;
    case SOUNDBIAS ... SOUNDBIAS+1:
        pointer = &biasCnt;
        break;
    case WAVE_RAM_OFFSET ... WAVE_RAM_OFFSET+0xF:
        waveRam[address - WAVE_RAM_OFFSET] = data;
        status();
        return;
    case FIFO_A ... FIFO_A+3:
        wordPointer = &fifoA;
        isWord=true;
        break;
    case FIFO_B ... FIFO_B+3:
        wordPointer = &fifoB;
        isWord=true;
        break;
    case FIFO_B+4 ... FIFO_B+10:
        DEBUG_OUT << "W: APU Unused Memory: "<<unsigned(address)<<endl;
        return;
    default:
        cout << "W: APU Undefined Memory: "<<unsigned(address)<<endl;
        exit(FAILED_DMA);
    }
    if(isWord)
        storeWord(address&0b11, wordPointer, data);
    else
        storeHalfWord(address&1, pointer, data);
}
