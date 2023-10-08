#ifndef COMMON_DS_H
#define COMMON_DS_H

const uint32_t N = 0x80000000;
const uint32_t Z = 0x40000000;
const uint32_t V = 0x10000000;

enum Opcode {
    NOT_INITIALISED,
    B,
    BX,
    AND,
    SUB,
    ADD,
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
    PUSH,
    POP,
    ADDSP,
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

#endif