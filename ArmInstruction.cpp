#include <sstream>
#include "FailureCodes.h"
#include "ArmInstruction.h"

ArmInstruction::~ArmInstruction()
{
}

ArmInstruction::ArmInstruction()
{
    opcode = NOT_INITIALISED;
}

ArmInstruction::ArmInstruction(Opcode operation){
    opcode = operation;
}

ArmInstruction::ArmInstruction(Opcode operation, int imm): ArmInstruction::ArmInstruction(operation){
    immediate = imm;
}

ArmInstruction::ArmInstruction(Opcode operation, int imm, int tempReg): ArmInstruction::ArmInstruction(operation, imm){
    regN = tempReg;
}

string ArmInstruction::toString(){
    stringstream stream;
    switch (opcode)
    {
    case B:
        stream << showbase << hex << "B " << immediate;
        return stream.str();
    case CMP:
        stream << showbase << "CMP R" << regN << hex << ", " << immediate;
        return stream.str();
    default:
        exit(FAILED_DECODED_TO_STRING);
    }
    return "Undefined";
}

Opcode ArmInstruction::getOpcode(){
    return opcode;
}

int ArmInstruction::getImmediate(){
    return immediate;
}

int ArmInstruction::getRegN(){
    return regN;
}
