#ifndef LOAD_PC_REL_H
#define LOAD_PC_REL_H

#include <iostream>
#include <string>
#include <sstream>
#include "Instruction.h"
#include "../FailureCodes.h"

using namespace std;

class LoadPCRelative : public ThumbInstruction
{
public:
    LoadPCRelative(char rD, int imm): ThumbInstruction(LDRPC, rD, imm){}

    static LoadPCRelative* decode(int opcode){
        char rD = (opcode>>8) & 0b111;
        int imm = opcode & 0xFF;
        return new LoadPCRelative(rD, imm);
    }

    string toString(){
        stringstream stream;
        stream << showbase << "LDR R" << unsigned(getRegDest()) <<", [PC"<< hex << ", " << getImmediate()<<"]";
        return stream.str();
    }
};

#endif
