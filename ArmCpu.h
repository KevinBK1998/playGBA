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
    void branch();
    void branchExchange();
    void psrTransfer();
    void logicalAND();
    void andImmediate();
    void andShifted();
    void subtract();
    void add();
    void addImmediate();
    void addShifted();
    void test();
    void testXOR();
    void compare();
    void cmpImmediate();
    void cmpShifted();
    void logicalOR();
    void bitClear();
    void move();
    void moveImmediate();
    void moveShifted();
    void loadReg();
    void storeReg();
    void loadMultipleReg();
    void storeMultipleReg();
    void reverseSubtract();
    bool canExecute(ArmInstruction*);
public:
    long time=0;
    ArmCpu(Registers*, Memory*);
    void decode();
    void execute();
    void step();
};

#endif
