#include <sstream>
#include "../FailureCodes.h"
#include "Instruction.h"

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

ArmInstruction::ArmInstruction(Condition cond, Opcode operation) : ArmInstruction::ArmInstruction(operation){
    condition = cond;
}

ArmInstruction::ArmInstruction(Opcode operation, char op1): ArmInstruction::ArmInstruction(operation){
    regN = op1;
}

ArmInstruction::ArmInstruction(Condition cond, Opcode operation, char op1): ArmInstruction::ArmInstruction(cond, operation){
    regN = op1;
}

ArmInstruction::ArmInstruction(Opcode operation, char op1, char destReg): ArmInstruction::ArmInstruction(operation, op1){
    regDest = destReg;
}

ArmInstruction::ArmInstruction(Condition cond, Opcode operation, char op1, char destReg): ArmInstruction::ArmInstruction(cond, operation, op1){
    regDest = destReg;
}

ArmInstruction::ArmInstruction(Opcode operation, char op1, char destReg, int imm): ArmInstruction::ArmInstruction(operation, op1, destReg){
    immediate = imm;
}

ArmInstruction::ArmInstruction(Condition cond, Opcode operation, char op1, char destReg, int imm): ArmInstruction::ArmInstruction(cond, operation, op1, destReg){
    immediate = imm;
}

ArmInstruction::ArmInstruction(Condition cond, Opcode operation, int imm): ArmInstruction::ArmInstruction(cond, operation){
    immediate = imm;
}

ArmInstruction::ArmInstruction(Opcode operation, char op1, int imm): ArmInstruction::ArmInstruction(operation, imm){
    regN = op1;
}

ArmInstruction::ArmInstruction(Condition cond, Opcode operation, char op1, int imm): ArmInstruction::ArmInstruction(cond, operation, imm){
    regN = op1;
}

ArmInstruction::ArmInstruction(Opcode operation, int imm, char destReg): ArmInstruction::ArmInstruction(operation, imm){
    regDest = destReg;
}

ArmInstruction::ArmInstruction(Condition cond, Opcode operation, int imm, char destReg): ArmInstruction::ArmInstruction(cond, operation, imm){
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

bool ArmInstruction::shouldUseImmediate(){
    return false;
}

string ArmInstruction::toString(){
    stringstream stream;
    switch (opcode)
    {
    default:
        exit(FAILED_DECODED_TO_STRING);
    }
    return "Undefined";
}

string ArmInstruction::getCondition(){
    return condition.toString();
}

Condition ArmInstruction::getPreCheck(){
    return condition;
}

Opcode ArmInstruction::getOpcode(){
    return opcode;
}

int ArmInstruction::getImmediate(){
    return immediate;
}

int ArmInstruction::getRegDest(){
    return regDest;
}

int ArmInstruction::getRegN(){
    return regN;
}

string Condition::toString(){
    switch (value)
    {
    case EQ:
        return "{eq}";
    case NE:
        return "{ne}";
    case CC:
        return "{cc}";
    case LT:
        return "{lt}";
    case ALWAYS:
        return "";
    default:
        cout << "COND = " << value << endl;
        exit(FAILED_DECODED_TO_STRING);
    }
    return "Never";
}
