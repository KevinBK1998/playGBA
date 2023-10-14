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
    void storeReg();
    void storeImmediateOffset();
    void storeHalfReg();
    void loadHalfReg();
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
    void pop();
    void branchExchange();
    void shiftLeft();
    void shiftRight();
    void test();
    void logicalOR();
    void compare();
    bool canExecute(int);
    bool canExecute(Condition);
public:
    ThumbCpu(Registers*, Memory*);
    void decode();
    void execute();
    void step();
};

#endif
