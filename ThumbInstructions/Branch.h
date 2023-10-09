#ifndef THUMB_BRANCH_H
#define THUMB_BRANCH_H

#include <iostream>
#include <sstream>
#include "Instruction.h"
#include "../FailureCodes.h"

using namespace std;

class ThumbBranch: public ThumbInstruction
{
private:
    Condition condition;
public:
    ThumbBranch(Condition cond, int imm): ThumbInstruction(B, imm){
        condition = cond;
    }

    static ThumbBranch* decode(int opcode){
        Condition cond = Condition((opcode>>8) & 0xF);
        int imm = opcode & 0xFF;
        if (imm & 0x80)
            imm |= 0xFFFFFF00;
        return new ThumbBranch(cond, imm);
    }

    Condition getPreCheck(){
        return condition;
    }

    string toString(){
        stringstream stream;
        switch (getOpcode())
        {
        case B:
            stream << showbase << "B" << condition.toString() << hex << " " << getImmediate() << " ("<<dec<<getImmediate()<<")";
            return stream.str();
        default:
            cout << "ERROR = " << getOpcode() << endl;
            exit(FAILED_DECODED_TO_STRING);
        }
        return "Undefined";
    }
};

#endif