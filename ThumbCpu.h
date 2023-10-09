#ifndef THUMP_CPU_H
#define THUMP_CPU_H

#include "Registers.h"
#include "Memory.h"
#include "ThumbInstructions/Instruction.h"

class ThumbCpu{
    Registers* reg;
    Memory* mem;
    ThumbInstruction* decodedInstruction = new ThumbInstruction();
    int generateFlags(int result);
    void move();
    void moveHigh();
    void moveN();
    void loadRegPCRelative();
    void storeReg();
    void storeHalfReg();
    void storeRegSPRelative();
    void add();
    void addSP();
    void branch();
    void condBranch();
    void longBranch();
    void push();
    void branchExchange();
    void shiftLeft();
    void shiftRight();
    void test();
    void logicalOR();
    bool canExecute(int);
    bool canExecute(Condition);
public:
    ThumbCpu(Registers* registers, Memory* memory);
    void decode();
    void execute();
    void step();
};

#endif
