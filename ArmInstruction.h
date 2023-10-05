#ifndef ARM_INSTR_H
#define ARM_INSTR_H

#include <string.h>
using namespace std;

enum Opcode {
    NOT_INITIALISED,
    B,
};
class ArmInstruction
{
private:
    Opcode opcode;
    int immediate;
public:
    ArmInstruction();
    ArmInstruction(Opcode);
    ArmInstruction(Opcode, int);
    ~ArmInstruction();
    string toString();
    Opcode getOpcode();
    int getImmediate();
};

#endif
