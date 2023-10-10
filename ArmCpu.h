#ifndef ARM_CPU_H
#define ARM_CPU_H

#include "Registers.h"
#include "Memory.h"
#include "ArmInstructions/Instruction.h"

class ArmCpu
{
private:
    Registers* reg;
    Memory* mem;
    ArmInstruction* decodedInstruction = new ArmInstruction();
    int setFlags(int);
    void branch();
    void branchExchange();
    void psrTransfer();
    void logicalAND();
    void subtract();
    void add();
    void testXOR();
    void compare();
    void logicalOR();
    void bitClear();
    void moveImmediate();
    void moveShifted();
    void loadReg();
    void storeReg();
    void loadMultipleReg();
    void storeMultipleReg();
    bool canExecute(int);
    bool canExecute(ArmInstruction*);
public:
    long time=0;
    ArmCpu(Registers*, Memory*);
    void decode();
    void execute();
    void step();
};

#endif
