#ifndef THUMB_SDT_H
#define THUMB_SDT_H

#include <iostream>
#include <string>
#include <sstream>
#include "../FailureCodes.h"
#include "Instruction.h"

using namespace std;

class ThumbSDT : public ThumbInstruction
{
private:
    char regBase;
    char regOffset;
public:
    ThumbSDT(Opcode opcode, char rO, char rB, char rD): ThumbInstruction(opcode, rD){
        regBase = rB;
        regOffset = rO;
    }

    static ThumbSDT* decode(int opcode){
        char op = (opcode>>10) & 0b11;
        if((opcode>>9) & 1) {
            cout << "LDR/STR OP special = " << hex << unsigned(op) << endl;
            exit(FAILED_TO_DECODE);
        }char rO = (opcode>>6) & 0b111;
        char rB = (opcode>>3) & 0b111;
        char rD = opcode & 0b111;
        switch (op)
        {
        case 0:
            return new ThumbSDT(STR, rO, rB, rD);
        default:
            cout << "LDR/STR OP = " << hex << unsigned(op) << endl;
            exit(FAILED_TO_DECODE);
            break;
        }
    }

    char getRegBase(){
        return regBase;
    }

    char getRegOffset(){
        return regOffset;
    }

    string toString(){
        stringstream stream;
        switch (getOpcode())
        {
        case STR:
            stream << showbase << "STR R" << unsigned(getRegDest()) <<", [R"<<unsigned(regBase) << ", R" << unsigned(regOffset)<<"]";
            return stream.str();
        default:
            cout << "ThumbSDT = " << hex << getOpcode() << endl;
            exit(FAILED_DECODED_TO_STRING);
        }
        return "Undefined";
    }
};

#endif
