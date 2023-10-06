#ifndef ARM_INSTR_H
#define ARM_INSTR_H

#include <string.h>
using namespace std;

enum Opcode {
    NOT_INITIALISED,
    B,
    TEQ,
    MRS,
    MSR,
    CMP,
    ORR,
    MOV,
    LDR,
};

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
public:
    ArmInstruction();
    ArmInstruction(Opcode);
    ArmInstruction(Opcode, int);
    ArmInstruction(Opcode, char);
    ArmInstruction(Opcode, char, char);
    ArmInstruction(Opcode, char, char, int);
    ArmInstruction(Opcode, char, int);
    ArmInstruction(Opcode, int, char);
    ArmInstruction(Opcode, char, int, int, char);
    ~ArmInstruction();
    Opcode getOpcode();
    int getImmediate();
    void setImmediate(int);
    int getRegDest();
    int getRegN();

    virtual string toString();
};

#endif
