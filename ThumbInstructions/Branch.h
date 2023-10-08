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
    char regSource;
public:
    ThumbBranch(Condition cond, int imm): ThumbInstruction(B, imm){
        condition = cond;
    }

    ThumbBranch(char regS): ThumbInstruction(BX){
        regSource = regS;
    }

    static ThumbBranch* decode(int opcode){
        Condition cond = Condition((opcode>>8) & 0xF);
        int imm = opcode & 0xFF;
        if (imm & 0x80)
            imm |= 0xFFFFFF00;
        return new ThumbBranch(cond, imm);
    }

    // decode for BX, bool value does not matter
    static ThumbBranch* decode(int opcode, bool exchange){
        char op = (opcode>>8) & 0b11;
        char rS = (opcode>>3) & 0xF;
        switch (op)
        {
        case 3:
            return new ThumbBranch(rS);
        default:
            exit(FAILED_TO_DECODE);
            break;
        }
    }

    Condition getPreCheck(){
        return condition;
    }

    char getRegSource(){
        return regSource;
    }

    string toString(){
        stringstream stream;
        switch (getOpcode())
        {
        case B:
            stream << showbase << "B" << condition.toString() << hex << " " << getImmediate() << " ("<<dec<<getImmediate()<<")";
            return stream.str();
        case BX:
            stream << showbase << "BX R" << unsigned(regSource);
            return stream.str();
        default:
            cout << "ERROR = " << getOpcode() << endl;
            exit(FAILED_DECODED_TO_STRING);
        }
        return "Undefined";
    }
};

#endif