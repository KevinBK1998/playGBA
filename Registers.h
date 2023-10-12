#ifndef REGISTERS_H
#define REGISTERS_H

#include <string>
#include "CommonDS.h"
#include "FailureCodes.h"

using namespace std;

enum Mode{
    SYSTEM_OR_USER,
    SUPERVISOR,
    INTERRUPT_REQUEST,
    ABORT,
    UNDEFINED,
};

#define SP 13
#define LR 14
#define PC 15

class Registers
{
private:
    // Registers 0-12
    int unbankedReg[13];
    // Registers 13(SP) and 14(LR)
    int bankedReg[10];
    int savedStatusReg[5];
    int currentStatusReg;
    int reg15 = WORD_SIZE;
    bool irqEnable;
    bool thumbMode;
    bool privilegedUser;
    Mode mode;
    int getBankedIndex(int index) {
        index -= 13;
        return index * 5 + mode;
    }

    string getMode() {
        switch (mode) {
            case SYSTEM_OR_USER:
                // if (privileged)
                    return "sys";
                // else
                    return "usr";
            case INTERRUPT_REQUEST:
                return "irq";
            case SUPERVISOR:
                return "svc";
            case ABORT:
                return "abt";
            case UNDEFINED:
                return "und";
            default:
                exit(PENDING_CODE);
                return "";
        }
    }

    void setControlBits();

public:
    Registers(/* args */);
    ~Registers();
    int getStepAmount();
    int getPC();
    void setPC(int);
    uint32_t getReg(int);
    void setReg(char, int);
    int getCPSR();
    void setCPSR(int);
    void setFlags(char, int);
    int getSPSR();
    void setSPSR(int);
    void branch(int);
    bool isThumbMode();
    void exchange(int);
    void step();
    void status();
};

#endif
