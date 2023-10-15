#ifndef COMMON_DS_H
#define COMMON_DS_H

#include <iostream>
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
    MOV_REG_ADDRESS,
    LDRH_E,
    STRH_E,
    ROR,
    EOR,
    MUL,
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

class ControlRegister16_t{
    uint16_t REG;
public:
    bool bitCheck(int);
    void storeReg(uint8_t, uint8_t);
    void storeReg(uint8_t, uint8_t, uint16_t);
    uint8_t loadReg(uint8_t);
    uint16_t getRegValue();
};

bool ControlRegister16_t::bitCheck(int bitNumber){
    if (bitNumber < 16)
        return (1<<bitNumber) & REG;
    return false;
}

void ControlRegister16_t::storeReg(uint8_t address, uint8_t data){
    if (address & 1){
        REG &= 0xFF;
        REG |= (data<<8);
    }
    else {
        REG &= 0xFF00;
        REG |= data;
    }
    DEBUG_OUT<<"IOREG: "<<unsigned(address)<<"\tData: "<<REG<<endl;
}

void ControlRegister16_t::storeReg(uint8_t address, uint8_t data, uint16_t mask){
    if (address & 1){
        REG &= (0xFF|mask);
        REG |= ((data<<8) & ~mask);
    }
    else {
        REG &= (0xFF00|mask);
        REG |= (data & ~mask);
    }
    DEBUG_OUT<<"IOREG: "<<unsigned(address)<<"\tData: "<<REG<<endl;
}

uint8_t ControlRegister16_t::loadReg(uint8_t address){
    if (address & 1)
        return REG>>8;
    else
        return REG;
}

uint16_t ControlRegister16_t::getRegValue(){
    return REG;
}

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

#define ROR(a,b) (((a)>>(b))|((a)<<(32 - (b))))
#define CARRY_ROR(a,b) (((a)>>((b)-1)) & 1)

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