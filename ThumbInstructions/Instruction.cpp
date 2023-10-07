#include "Instruction.h"

ThumbInstruction::ThumbInstruction(){
    opcode = NOT_INITIALISED;
}

ThumbInstruction::ThumbInstruction(Opcode operation){
    opcode = operation;
}

Opcode ThumbInstruction::getOpcode(){
    return opcode;
}

string ThumbInstruction::toString(){
    return "Undefined";
}