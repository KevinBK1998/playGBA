#ifndef ARM_BRANCH_H
#define ARM_BRANCH_H

#include <iostream>
#include <sstream>
#include "../FailureCodes.h"
#include "Instruction.h"

class Branch: public ArmInstruction{
private:
    bool linkFlag;
public:
    Branch(Condition cond, int imm, bool link): ArmInstruction(cond, B, imm){
        linkFlag = link;
    }

    Branch(Condition cond, char rN, bool link): ArmInstruction(cond, BX, rN){
        linkFlag = link;
    }

    static Branch* decode(int opcode){
        Condition cond = Condition((opcode >> 28) & 0xF);
        bool linkFlag = (opcode>>24) & 0b1 !=0;
        int imm = opcode & 0xFFFFFF;
        if ((imm >> 23) & 0b1) // Sign bit
            imm |= 0xFF000000;
        return new Branch(cond, imm, linkFlag);
    }

    // decode for Branch Exchange, bool value does not matter
    static Branch* decode(int opcode, bool exchange){
        Condition cond = Condition((opcode >> 28) & 0xF);
        bool linkFlag = ((opcode>>4) & 0b1111) == 0b11;
        char rN = opcode & 0b1111;
        return new Branch(cond, rN, linkFlag);
    }

    bool shouldSavePC(){
        return linkFlag;
    }

    string toString(){
        stringstream stream;
        switch (getOpcode())
        {
        case B:
            stream<<hex<<showbase<<(linkFlag?"BL":"B")<<getCondition()<<" "<<getImmediate()<<dec<<" ("<<getImmediate()*4<<")";
            return stream.str();
        case BX:
            stream<<(linkFlag?"BLX":"BX")<<getCondition()<<" R"<<getRegN();
            return stream.str();
        default:
            exit(FAILED_DECODED_TO_STRING);
        }
        return "Undefined";
    }
};

#endif