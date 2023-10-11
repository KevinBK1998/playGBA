#ifndef THUMB_SDT_IMM_H
#define THUMB_SDT_IMM_H

#include <iostream>
#include <string>
#include <sstream>
#include "Instruction.h"
#include "../FailureCodes.h"

using namespace std;

class SDTHalfImmediate : public ThumbInstruction
{
private:
    char regBase;
public:
    SDTHalfImmediate(Opcode opcode, int offset, char rB, char rD): ThumbInstruction(opcode, rD, offset){
        regBase = rB;
    }

    static SDTHalfImmediate* decode(int opcode){
        Opcode op = STRH;
        if((opcode>>11) & 1) 
            op = LDRH;
        int offset = (opcode>>6) & 0b11111;
        char rB = (opcode>>3) & 0b111;
        char rD = opcode & 0b111;
        return new SDTHalfImmediate(op, offset, rB, rD);
    }

    char getRegBase(){
        return regBase;
    }

    string toString(){
        stringstream stream;
        switch (getOpcode())
        {
        case STRH:
            stream << showbase << "STRH R" << unsigned(getRegDest()) <<", [R"<<unsigned(regBase);
            if(getImmediate())
                stream<<", "<<hex<<showbase<< getImmediate();
            stream<<"]";
            break;
        default:
            cout << "SDTHalfImmediate = " << hex << getOpcode() << endl;
            exit(FAILED_DECODED_TO_STRING);
        }
        return stream.str();
    }
};

#endif
