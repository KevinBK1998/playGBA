#ifndef COMMON_DS_H
#define COMMON_DS_H

const uint32_t N = 0x80000000;
const uint32_t Z = 0x40000000;
const uint32_t V = 0x10000000;

#define BYTE_SIZE 1
#define HALFWORD_SIZE 2
#define WORD_SIZE 4

enum Opcode {
    NOT_INITIALISED,
    B,
    B_WORD,
    BX,
    AND,
    SUB,
    ADD,
    TEQ,
    TST,
    MRS,
    MSR,
    CMP,
    ORR,
    MOV,
    MVN,
    LDR,
    STR,
    LDRH,
    STRH,
    LDM,
    STM,
    PUSH,
    POP,
    LDRPC,
    LDRSP,
    STRSP,
    ADDSP,
    LSL,
    LSR,
    MOV_HI,
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
    std::string toString();
};

#endif