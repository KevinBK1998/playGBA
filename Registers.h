#ifndef REGISTERS_H
#define REGISTERS_H

#include <string>
#include "ArmInstructions/FailureCodes.h"

using namespace std;

enum Mode{
    SYSTEM_OR_USER,
    SUPERVISOR,
    INTERRUPT_REQUEST,
    ABORT,
    UNDEFINED,
};

#define WORD_SIZE 4
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

    void setControlBits() {
        irqEnable = (currentStatusReg & 0x80) == 0;
        thumbMode = (currentStatusReg & 0x20) != 0;
        int modeMeta = currentStatusReg & 0x1F;
        switch (modeMeta) {
            case 0:
            case 16:
                mode = SYSTEM_OR_USER;
                privilegedUser = false;
                break;
            case 2:
            case 18:
                mode = INTERRUPT_REQUEST;
                break;
            case 3:
            case 19:
                mode = SUPERVISOR;
                break;
            case 23:
                mode = ABORT;
                break;
            case 27:
                mode = UNDEFINED;
                break;
            case 31:
                mode = SYSTEM_OR_USER;
                privilegedUser = true;
                break;
            default:
                exit(PENDING_CODE);
        }
    }

public:
    Registers(/* args */);
    ~Registers();
    int getPC();
    void setPC(int);
    int getReg(int);
    void setReg(char, int);
    int getCPSR();
    void setCPSR(int);
    int getSPSR();
    void setSPSR(int);
    void branch(int);
    bool isThumbMode();
    void exchange(bool);
    void step();
    void status();
};

#endif
