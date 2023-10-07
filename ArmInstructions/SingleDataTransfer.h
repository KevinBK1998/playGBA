#ifndef ARM_SDT_H
#define ARM_SDT_H

#include <iostream>
#include <sstream>
#include <string>
#include "../FailureCodes.h"
#include "Instruction.h"
using namespace std;

class SingleDataTransfer : public ArmInstruction
{
private:
    bool preFlag;
    bool addFlag;
    bool byteTransfer;
public:
    SingleDataTransfer():ArmInstruction(){}
    ~SingleDataTransfer(){}

    SingleDataTransfer(Condition cond, Opcode operation, char op1, int flags, int imm, char destReg):ArmInstruction(cond, operation, op1, destReg, imm){
        preFlag = ((flags >> 2) & 1) != 0;//P
        addFlag = ((flags >> 1) & 1) != 0;//U
        byteTransfer = (flags & 1) != 0;
    }

    static SingleDataTransfer* decodeSDT(int opcode){
        Condition cond = Condition((opcode >> 28) & 0xF);
        if (cond.value == 0xF)
            cout << "Incomplete PLD{T} Rd, <Address> = " << opcode << endl;
        int flags = (opcode >> 22) & 0xF;
        int imm = opcode & 0xFFF;
        char rN = (opcode >> 16) & 0xF;
        char rDest = (opcode >> 12) & 0xF;
        Opcode operation = ((opcode >> 20) & 1)? LDR : STR;
        return new SingleDataTransfer(cond, operation, rN, flags, imm, rDest);
    }

    bool isByteTransfer(){
        return byteTransfer;
    }

    string toString(){
        stringstream stream;
        switch (getOpcode())
        {
        case LDR:
            stream << "LDR" << (byteTransfer?"B":"") << getCondition() <<" R" << getRegDest() << ", [R"<< getRegN() << showbase << hex << ", " << getImmediate() <<"]";
            return stream.str();
        case STR:
            stream << "STR" << (byteTransfer?"B":"") << getCondition() <<" R" << getRegDest() << ", [R"<< getRegN() << showbase << hex << ", " << getImmediate() <<"]";
            return stream.str();
        default:
            exit(FAILED_DECODED_TO_STRING);
        }
        return "Undefined";
    }
};

#endif
