#ifndef ARM_INSTR_H
#define ARM_INSTR_H

#include <string.h>
using namespace std;

enum Opcode {
    NOT_INITIALISED,
    B,
    CMP,
};
class ArmInstruction
{
private:
    Opcode opcode;
    int regN;
    int immediate;
public:
    ArmInstruction();
    ArmInstruction(Opcode);
    ArmInstruction(Opcode, int);
    ArmInstruction(Opcode, int, int);
    ~ArmInstruction();
    string toString();
    Opcode getOpcode();
    int getImmediate();
    int getRegN();
};

#endif
