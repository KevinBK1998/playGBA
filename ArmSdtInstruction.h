#ifndef ARM_SDT_INSTR_H
#define ARM_SDT_INSTR_H

#include <string.h>
#include "ArmInstruction.h"
using namespace std;

class ArmSdtInstruction : public ArmInstruction
{
private:
    bool preFlag;
    bool addFlag;
    bool byteTransfer;
public:
    ArmSdtInstruction():ArmInstruction(){}
    ArmSdtInstruction(Opcode operation, char op1, int flags, int data, char destReg):ArmInstruction(operation, op1, destReg){
        preFlag = ((flags >> 2) & 1) != 0;//P
        addFlag = ((flags >> 1) & 1) != 0;//U
        byteTransfer = (flags & 1) != 0;//B
        setImmediate(data & 0xFFF);
    }
    ~ArmSdtInstruction(){}
};

#endif