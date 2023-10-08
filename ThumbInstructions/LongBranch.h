#ifndef THUMB_LBRANCH_H
#define THUMB_LBRANCH_H

#include <iostream>
#include <sstream>
#include "Instruction.h"
#include "../FailureCodes.h"

using namespace std;

class ThumbLongBranch: public ThumbInstruction
{
private:
    bool firstOpcode;
    bool exchange;
public:
    ThumbLongBranch(int imm): ThumbInstruction(B_WORD, imm){
        firstOpcode = true;;
    }
    ThumbLongBranch(bool flag, int imm): ThumbInstruction(B_WORD, imm){
        firstOpcode = false;
        exchange = flag;
    }

    static ThumbLongBranch* decode(int opcode){
        if ((opcode>>11) == 0b11110){
            int imm = (opcode & 0x7FF)<<12;
            imm = (opcode & 0x400)? (imm|0xFF800000) : imm;
            return new ThumbLongBranch(imm);
        }
        bool flag = (opcode>>11) == 0b11101;
        int imm = (opcode & 0x7FF)<<1;
        return new ThumbLongBranch(flag, imm);
    }

    bool isFirstOpcode(){
        return firstOpcode;
    }

    bool shouldExchange(){
        return exchange;
    }

    string toString(){
        stringstream stream;
        switch (getOpcode())
        {
        case B_WORD:
            stream << showbase <<(firstOpcode?"first:":"second:")<< "BL "<< hex << getImmediate() << " ("<<dec<<getImmediate()<<")";
            return stream.str();
        default:
            cout << "ERROR = " << getOpcode() << endl;
            exit(FAILED_DECODED_TO_STRING);
        }
        return "Undefined";
    }
};

#endif