#include <iostream>
#include "FailureCodes.h"
using namespace std;
const int POST_BOOT_FLAG = 0x300;
const int IME = 0x208;

class IORegisters
{
private:
    uint8_t postBootFlag;
    bool ime;
public:
    IORegisters(/* args */);
    ~IORegisters();
    uint8_t read8(int address) {
        if (address == POST_BOOT_FLAG)
            return postBootFlag;
        else if (address == IME)
            return (ime ? 1 : 0);
        cout << "R: Unused Memory: 0x"<<address<<endl;
        exit(FAILED_DMA);
    }

    void write8(int address, uint8_t data) {
        if (address == POST_BOOT_FLAG)
            postBootFlag = data;
        else if (address == IME)
            ime = (data & 1) != 0;
        else{
            cout << "W:Unknown Memory: 0x"<<address<<endl;
            exit(FAILED_DMA);
        }
    }
};

IORegisters::IORegisters(/* args */)
{
}

IORegisters::~IORegisters()
{
}

