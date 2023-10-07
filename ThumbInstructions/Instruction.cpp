#include "Instruction.h"

ThumbInstruction::ThumbInstruction(){
    opcode = NOT_INITIALISED;
}

ThumbInstruction::ThumbInstruction(Opcode operation){
    opcode = operation;
}

ThumbInstruction::ThumbInstruction(Opcode operation, char regD): ThumbInstruction::ThumbInstruction(operation){
    regDest = regD;
}

ThumbInstruction::ThumbInstruction(Opcode operation, char regD, int imm): ThumbInstruction::ThumbInstruction(operation, regD){
    immediate = imm;
}

Opcode ThumbInstruction::getOpcode(){
    return opcode;
}

char ThumbInstruction::getRegDest(){
    return regDest;
}

int ThumbInstruction::getImmediate(){
    return immediate;
}

string ThumbInstruction::toString(){
    return "Undefined";
}