#include <iostream>
#include "Instruction.h"
#include "../FailureCodes.h"

using namespace std;

ThumbInstruction::ThumbInstruction(){
    opcode = NOT_INITIALISED;
}

ThumbInstruction::ThumbInstruction(Opcode operation){
    opcode = operation;
}

ThumbInstruction::ThumbInstruction(Opcode operation, char regD): ThumbInstruction::ThumbInstruction(operation){
    regDest = regD;
}

ThumbInstruction::ThumbInstruction(Opcode operation, int imm): ThumbInstruction::ThumbInstruction(operation){
    immediate = imm;
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

bool ThumbInstruction::useImmediateOffset(){
    return false;
}

bool ThumbInstruction::useImmediate(){
    return false;
}

string ThumbInstruction::toString(){
    exit(FAILED_DECODED_TO_STRING);
    return "Undefined";
}