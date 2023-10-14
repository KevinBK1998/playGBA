#ifndef COMMON_DS_H
#define COMMON_DS_H

#include <sstream>
using namespace std;

#define DEBUG_OUT (DEBUG_LOGS? cout : nop)

const uint32_t N = 0x80000000;
const uint32_t Z = 0x40000000;
const uint32_t C = 0x20000000;
const uint32_t V = 0x10000000;
const int PAL_RAM_OFFSET = 0x5000000;
const int VRAM_OFFSET = 0x6000000;
const int OAM_OFFSET = 0x7000000;
const int PAL_RAM_SIZE = 0x400;
const int VRAM_SIZE = 0x18000;
const int OAM_SIZE = 0x400;
const char NZ = 0xC0;
const char NZC = 0xE0;
const char NZCV = 0xF0;
bool DEBUG_LOGS = false;

// Screen Size
const int OSCREEN_WIDTH = 240;
const int OSCREEN_HEIGHT = 160;
const int INITIAL_ZOOM = 2;
const int SCREEN_WIDTH = INITIAL_ZOOM*OSCREEN_WIDTH;
const int SCREEN_HEIGHT = INITIAL_ZOOM*OSCREEN_HEIGHT;

stringstream nop;

#define BYTE_SIZE 1
#define HALFWORD_SIZE 2
#define WORD_SIZE 4

enum Opcode {
    NOT_INITIALISED,
    B,
    B_WORD,
    B_COND,
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
    BIC,
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
    CMP_HI,
};

enum ShiftType{
    ShiftLeft,
    ShiftRight,
    ArithmeticShiftRight,
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

int generateShiftFlags(bool carry, int result){
    int flags = 0;
    if ((result>>31) & 1){
        flags |= N;
        DEBUG_OUT<<"N ";
    }
    if ((int) result == 0){
        flags |= Z;
        DEBUG_OUT<<"Z ";
    }
    if (carry){
        flags |= C;
        DEBUG_OUT<<"C ";
    }
    DEBUG_OUT<<"flags = "<< flags<< endl;
    return flags;
}

int generateFlags(long result){
    int flags = 0;
    if ((result>>31) & 1){
        flags |= N;
        DEBUG_OUT<<"N ";
    }
    if ((int) result == 0){
        flags |= Z;
        DEBUG_OUT<<"Z ";
    }
    if ((result>>32) & 1){
        flags |= C;
        DEBUG_OUT<<"C ";
    }
    DEBUG_OUT<<"flags = "<< flags<< endl;
    return flags;
}

int generateFlags(int op1, int op2, long result){
    int flags = generateFlags(result);
    bool op1Sign = op1 < 0;
    bool op2Sign = op2 < 0;
    bool resultSign = (result>>31)&1;
    if (op1Sign == op2Sign && op1Sign!=resultSign){
        flags |= V;
        DEBUG_OUT<<"V ";
    }
    DEBUG_OUT<<"flags = "<< flags<< endl;
    return flags;
}

#endif