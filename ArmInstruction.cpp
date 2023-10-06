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

ArmInstruction::ArmInstruction(Opcode operation, char op1): ArmInstruction::ArmInstruction(operation){
    regN = op1;
}

ArmInstruction::ArmInstruction(Opcode operation, char op1, char destReg): ArmInstruction::ArmInstruction(operation, op1){
    regDest = destReg;
}

ArmInstruction::ArmInstruction(Opcode operation, int imm): ArmInstruction::ArmInstruction(operation){
    immediate = imm;
}

ArmInstruction::ArmInstruction(Opcode operation, char op1, int imm): ArmInstruction::ArmInstruction(operation, imm){
    regN = op1;
}

ArmInstruction::ArmInstruction(Opcode operation, int imm, char destReg): ArmInstruction::ArmInstruction(operation, imm){
    regDest = destReg;
}

ArmInstruction::ArmInstruction(Opcode operation, char op1, int flags, int data, char destReg): ArmInstruction::ArmInstruction(operation){
    preFlag = ((flags >> 2) & 1) != 0;//P
    addFlag = ((flags >> 1) & 1) != 0;//U
    byteTransfer = (flags & 1) != 0;//B
    immediate = data & 0xFFF;
    regN = op1;
    regDest = destReg;
}

string ArmInstruction::toString(){
    stringstream stream;
    switch (opcode)
    {
    case B:
        stream << showbase << hex << "B " << immediate;
        return stream.str();
    case TEQ:
        stream << showbase << "TEQ R" << regN << hex << ", " << immediate;
        return stream.str();
    case CMP:
        stream << showbase << "CMP R" << regN << hex << ", " << immediate;
        return stream.str();
    case MOV:
        stream << showbase << "MOV R" << regDest << hex << ", " << immediate;
        return stream.str();
    case LDR:
        stream << showbase << "LDR R" << regDest << ", [R"<<regN<< hex << ", " << immediate<<"]";
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

void ArmInstruction::setImmediate(int data){
    immediate=data;
}

int ArmInstruction::getRegDest(){
    return regDest;
}

int ArmInstruction::getRegN(){
    return regN;
}
