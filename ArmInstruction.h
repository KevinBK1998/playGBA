#ifndef ARM_INSTR_H
#define ARM_INSTR_H

#include <string.h>
using namespace std;

enum Opcode {
    NOT_INITIALISED,
    B,
    CMP,
    MOV,
};
class ArmInstruction
{
private:
    Opcode opcode;
    int regDest;
    int regN;
    int immediate;
public:
    ArmInstruction();
    ArmInstruction(Opcode);
    ArmInstruction(Opcode, int);
    ArmInstruction(Opcode, char, int);
    ArmInstruction(Opcode, int, char);
    ~ArmInstruction();
    string toString();
    Opcode getOpcode();
    int getImmediate();
    int getRegDest();
    int getRegN();
};

#endif
