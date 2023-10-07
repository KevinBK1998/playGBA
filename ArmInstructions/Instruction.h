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
    STR,
    LDM,
    STM,
};

enum ConditionConstant {
    EQ,
    NE,
    CS,
    CC,
    MI,
    PL,
    VS, //Signed Overflow
    VC, //No signed Overflow
    HI, //Unsigned higher
    LS, //Unsigned lower or same
    GE,
    LT,
    GT,
    LE,
    ALWAYS,
};

class Condition{
public:
    int value;
    Condition(){
        value = 0xF;
    }
    Condition(int cond){
        value = cond;
    }
    ~Condition(){}
    string toString();
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
    Condition condition;
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
    Opcode getOpcode();
    int getImmediate();
    int getRegDest();
    int getRegN();

    virtual string toString();
};

#endif
