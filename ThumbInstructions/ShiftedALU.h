#ifndef THUMB_SHIFT_ALU_H
#define THUMB_SHIFT_ALU_H

#include <iostream>
#include <sstream>
#include "Instruction.h"
#include "../FailureCodes.h"

using namespace std;

class ShiftedALU: public ThumbInstruction
{
private:
    char regSource;
public:
    ShiftedALU(Opcode operation, char regD, char regS, int offset): ThumbInstruction(operation, regD, offset){
        regSource = regS;
    }

    static ShiftedALU* decode(int opcode){
        int op = (opcode>>11)&0b11;
        int offset = (opcode>>6)&0x1F;
        char regS = (opcode>>3)&0b111;
        char regD = opcode&0b111;
        switch (op)
        {
        case 0:
            return new ShiftedALU(LSL, regD, regS, offset);
        default:
            cout << "ALU = " << unsigned(op) << endl;
            exit(FAILED_TO_DECODE);
            break;
        }
    }

    char getRegSource(){
        return regSource;
    }
    
    string toString(){
        stringstream stream;
        switch (getOpcode())
        {
        case LSL:
            stream << showbase << "LSL R" << unsigned(getRegDest()) << ", R" << unsigned(regSource) << hex << ", " << getImmediate();
            return stream.str();
        default:
            cout << "ALU = " << hex << getOpcode() << endl;
            exit(FAILED_DECODED_TO_STRING);
        }
        return "Undefined";
    }
};

#endif
