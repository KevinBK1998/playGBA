#ifndef THUMP_CPU_H
#define THUMP_CPU_H

#include "Registers.h"
#include "Memory.h"
#include "ThumbInstructions/Instruction.h"

class ThumbCpu{
    Registers* reg;
    Memory* mem;
    ThumbInstruction* decodedInstruction = new ThumbInstruction();
    void move();
    void moveHigh();
    void moveN();
    void loadRegPCRelative();
    void str();
    void ldr();
    void storeReg();
    void loadReg();
    void storeImmediateOffset();
    void loadImmediateOffset();
    void storeHalfReg();
    void loadHalfReg();
    void storeHalfRegExtended();
    void loadHalfRegExtended();
    void storeRegSPRelative();
    void add();
    void addRegWithImmediate();
    void addImmediate();
    void sub();
    void subRegWithImmediate();
    void addSP();
    void branch();
    void condBranch();
    void longBranch();
    void push();
    void storeMultipleReg();
    void pop();
    void loadMultipleReg();
    void branchExchange();
    void shiftLeft();
    void shiftRight();
    void test();
    void logicalOR();
    void compare();
    void compareImmediate();
    void compareReg();
    void compareHigh();
    void moveRegAddress();
    bool canExecute(Condition);
public:
    ThumbCpu(Registers*, Memory*);
    void decode();
    void execute();
    void step();
};

#endif
