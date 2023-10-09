#ifndef THUMB_SDT_IMM_H
#define THUMB_SDT_IMM_H

#include <iostream>
#include <string>
#include <sstream>
#include "Instruction.h"
#include "../FailureCodes.h"

using namespace std;

class SDTThumbIMM : public ThumbInstruction
{
private:
    char regBase;
public:
    SDTThumbIMM(Opcode opcode, int offset, char rB, char rD): ThumbInstruction(opcode, rD, offset){
        regBase = rB;
    }

    static SDTThumbIMM* decode(int opcode){
        Opcode op = STRH;
        if((opcode>>11) & 1) 
            op = LDRH;
        int offset = (opcode>>6) & 0b1111;
        char rB = (opcode>>3) & 0b111;
        char rD = opcode & 0b111;
        return new SDTThumbIMM(op, offset, rB, rD);
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
            cout << "SDTThumbIMM = " << hex << getOpcode() << endl;
            exit(FAILED_DECODED_TO_STRING);
        }
        return stream.str();
    }
};

#endif
