#ifndef SDT_SP_REL_H
#define SDT_SP_REL_H

#include <iostream>
#include <string>
#include <sstream>
#include "Instruction.h"
#include "../FailureCodes.h"

using namespace std;

class SDTRelativeSP : public ThumbInstruction
{
public:
    SDTRelativeSP(Opcode opcode, char rD, int imm): ThumbInstruction(opcode, rD, imm){}

    static SDTRelativeSP* decode(int opcode){
        Opcode operation;
        if((opcode>>11) & 0b1)
            operation=LDRSP;
        else
            operation=STRSP;
        char rD = (opcode>>8) & 0b111;
        int imm = opcode & 0xFF;
        return new SDTRelativeSP(operation, rD, imm);
    }

    string toString(){
        stringstream stream;
        switch (getOpcode())
        {
        case STRSP:
            stream << showbase << "STR R" << unsigned(getRegDest()) <<", [SP";
            if(getImmediate())
                stream<<", "<<hex<<showbase<< getImmediate();
            stream<<"]";
            return stream.str();
        default:
            cout << "SDTRelativeSP = " << hex << getOpcode() << endl;
            exit(FAILED_DECODED_TO_STRING);
        }
        return "Undefined";
    }
};

#endif
