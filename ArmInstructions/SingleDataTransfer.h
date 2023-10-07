#ifndef ARM_SDT_H
#define ARM_SDT_H

#include <string.h>
#include "Instruction.h"
using namespace std;

class ArmSdtInstruction : public ArmInstruction
{
private:
    bool preFlag;
    bool addFlag;
    bool byteTransfer;
public:
    ArmSdtInstruction():ArmInstruction(){}
    ArmSdtInstruction(Condition cond, Opcode operation, char op1, int flags, int data, char destReg):ArmInstruction(cond, operation, op1, destReg){
        preFlag = ((flags >> 2) & 1) != 0;//P
        addFlag = ((flags >> 1) & 1) != 0;//U
        byteTransfer = (flags & 1) != 0;//B
        setImmediate(data & 0xFFF);
    }
    ~ArmSdtInstruction(){}
};

#endif
