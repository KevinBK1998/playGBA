#include <iostream>
#include "FailureCodes.h"
using namespace std;
const int POST_BOOT_FLAG = 0x300;
const int IME = 0x208;

class IORegisters
{
private:
    uint16_t lcdControl;
    uint16_t ime;
    bool postBootFlag;
public:
    IORegisters(/* args */){}
    ~IORegisters(){}

    uint8_t read8(int address) {
        if (address == POST_BOOT_FLAG)
            return postBootFlag;
        else if (address == IME)
            return (ime ? 1 : 0);
        cout << "R: Unused Memory: 0x"<<address<<endl;
        exit(FAILED_DMA);
    }

    void write8(int address, uint8_t data) {
        if (!address){
            lcdControl &= 0xFF00;
            lcdControl |= data;
        }
        else if (address == 1){
            lcdControl &= 0xFF;
            lcdControl |= (data<<8);
        }
        else if (address == POST_BOOT_FLAG)
            postBootFlag = data? true: false;
        else if (address == IME){
            ime &= 0xFF00;
            ime |= data;
        }
        else if (address == IME+1){
            ime &= 0xFF;
            ime |= (data<<8);
        }
        else{
            cout << "W:Unknown Memory: "<<address<<endl;
            exit(FAILED_DMA);
        }
        cout<<"LCDCTRL: "<<lcdControl<<",IME: "<<ime<<",PBOOT: "<<postBootFlag<<endl;
    }
};
