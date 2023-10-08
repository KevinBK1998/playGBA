#ifndef ARM_INSTR_H
#define ARM_INSTR_H

#include <string.h>
#include "../CommonDS.h"
using namespace std;

class ArmInstruction
{
private:
    Opcode opcode;
    int regDest;
    int regN;
    int immediate;
    bool preFlag;
    bool addFlag;
    bool byteTransfer;
    Condition condition;
public:
    ArmInstruction();
    ArmInstruction(Opcode);
    ArmInstruction(Condition, Opcode);
    ArmInstruction(Condition, Opcode, int);
    ArmInstruction(Opcode, char);
    ArmInstruction(Condition, Opcode, char);
    ArmInstruction(Opcode, char, char);
    ArmInstruction(Condition, Opcode, char, char);
    ArmInstruction(Opcode, char, char, int);
    ArmInstruction(Condition, Opcode, char, char, int);
    ArmInstruction(Opcode, char, int);
    ArmInstruction(Condition, Opcode, char, int);
    ArmInstruction(Opcode, int, char);
    ArmInstruction(Condition, Opcode, int, char);
    ArmInstruction(Opcode, char, int, int, char);
    ~ArmInstruction();
    string getCondition();
    Condition getPreCheck();
    Opcode getOpcode();
    int getImmediate();
    int getRegDest();
    int getRegN();

    virtual string toString();
};

#endif
