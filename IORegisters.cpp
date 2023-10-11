#include <iostream>
#include "FailureCodes.h"
#include "CommonDS.h"
using namespace std;

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
    bool postBootFlag;
    bool checkForAddress(int address, int IOreg, int size){
        return address>=IOreg && address<IOreg+size;
    }
    void storeHalfWord(int address, uint16_t* var, uint8_t data){
        if (address){
            *(var) &= 0xFF;
            *(var) |= (data<<8);
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
        cout << "R: IORegisters Undefined Memory: 0x"<<address<<endl;
        exit(FAILED_DMA);
    }

    void write8(int address, uint8_t data) {
        if (checkForAddress(address, IE, HALFWORD_SIZE))
            storeHalfWord(address&1, &ie, data);
        else if (checkForAddress(address, IF, HALFWORD_SIZE))
            storeHalfWord(address&1, &i_f, data);
        else if (checkForAddress(address, WAITCNT, HALFWORD_SIZE))
            storeHalfWord(address&1, &waitCnt, data);
        else if (address > WAITCNT && address < IME )
            cout << "W: IORegisters Unused Memory: "<<address<<endl;
        else if (checkForAddress(address, IME, HALFWORD_SIZE))
            storeHalfWord(address&1, &ime, data);
        else if (address > IME && address < POST_BOOT_FLAG )
            cout << "W: IORegisters Unused Memory: "<<address<<endl;
        // else if (address == POST_BOOT_FLAG)
        //     postBootFlag = data? true: false;
        else{
            cout << "W: IORegisters Undefined Memory: "<<address<<endl;
            exit(FAILED_DMA);
        }
        if (address&1){
            cout<<"IOREG:"<<endl;
            cout<<"IE: "<<ie<<"\tIF: "<<i_f<<"\tWAITCNT: "<<waitCnt<<"\tIME: "<<ime<<"\tPBOOT: "<<postBootFlag<<endl<<endl;
        }
    }
};
