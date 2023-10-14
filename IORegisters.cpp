#include <iostream>
#include "FailureCodes.h"
#include "CommonDS.h"
using namespace std;

const int DMA0SAD = 0xB0;
const int DMA_END = 0xE0;
const int TM0CNT_L = 0x100;
const int TIMER_END = 0x110;
const int SIODATA32 = 0x120;
const int SERIAL1_END = 0x12C;
const int KEYINPUT = 0x130;
const int RCNT = 0x134;
const int SERIAL2_END = 0x15A;
const int IE = 0x200;
const int IF = 0x202;
const int WAITCNT = 0x204;
const int IME = 0x208;
const int POST_BOOT_FLAG = 0x300;

class IORegisters
{
private:
    uint16_t ie;
    uint16_t i_f;
    uint16_t waitCnt;
    uint16_t ime;
    uint8_t dma[48];
    uint8_t timer[16];
    uint8_t serial[12];
    uint8_t serial2[0x26];
    uint8_t postBootFlag;
    bool checkForAddress(int address, int IOreg, int size){
        return address>=IOreg && address<IOreg+size;
    }
    void storeHalfWord(int address, uint16_t* var, uint8_t data){
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
public:
    uint8_t read8(int address) {
        if (address == POST_BOOT_FLAG)
            return postBootFlag;
        // else if (address == IME)
        //     return ime;
        cout << "R: IORegisters Undefined Memory: "<<address<<endl;
        exit(FAILED_DMA);
    }

    void write8(int address, uint8_t data) {
        if (address >= DMA0SAD && address < DMA_END){
            DEBUG_OUT << "W: IORegisters DMA Memory: "<<address<<endl;
            dma[address-DMA0SAD]=data;
        }
        else if (address >= DMA_END && address < TM0CNT_L)
            DEBUG_OUT << "W: IORegisters Unused Memory: "<<address<<endl;
        else if (address >= TM0CNT_L && address < TIMER_END){
            DEBUG_OUT << "W: IORegisters TIMER Memory: "<<address<<endl;
            timer[address-TM0CNT_L]=data;
        }
        else if (address >= TIMER_END && address < SIODATA32)
            DEBUG_OUT << "W: IORegisters Unused Memory: "<<address<<endl;
        else if (address >= SIODATA32 && address < SERIAL1_END){
            DEBUG_OUT << "W: IORegisters SERIAL_1 Memory: "<<address<<endl;
            serial[address-SIODATA32]=data;
        }
        else if (address >= SERIAL1_END && address < KEYINPUT)
            DEBUG_OUT << "W: IORegisters Unused Memory: "<<address<<endl;
        else if (address >= RCNT && address < SERIAL2_END){
            DEBUG_OUT << "W: IORegisters SERIAL_2 Memory: "<<address<<endl;
            serial[address-RCNT]=data;
        }
        else if (address >= SERIAL2_END && address < IE)
            DEBUG_OUT << "W: IORegisters Unused Memory: "<<address<<endl;
        else if (checkForAddress(address, IE, HALFWORD_SIZE))
            storeHalfWord(address&1, &ie, data);
        else if (checkForAddress(address, IF, HALFWORD_SIZE))
            storeHalfWord(address&1, &i_f, data);
        else if (checkForAddress(address, WAITCNT, HALFWORD_SIZE))
            storeHalfWord(address&1, &waitCnt, data);
        else if (address > WAITCNT && address < IME )
            DEBUG_OUT << "W: IORegisters Unused Memory: "<<address<<endl;
        else if (checkForAddress(address, IME, HALFWORD_SIZE))
            storeHalfWord(address&1, &ime, data);
        else if (address > IME && address < POST_BOOT_FLAG )
            DEBUG_OUT << "W: IORegisters Unused Memory: "<<address<<endl;
        else if (address == POST_BOOT_FLAG)
            postBootFlag = data;
        else{
            cout << "W: IORegisters Undefined Memory: "<<address<<endl;
            exit(FAILED_DMA);
        }
    }

    void status(){
        DEBUG_OUT<<"IOREG:"<<endl;
        DEBUG_OUT<<"IE: "<<ie<<"\tIF: "<<i_f<<"\tWAITCNT: "<<waitCnt<<"\tIME: "<<ime<<"\tPBOOT: "<<postBootFlag<<endl<<endl;
    }
};
