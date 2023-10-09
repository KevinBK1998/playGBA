#ifndef THUMB_ADD_SP_H
#define THUMB_ADD_SP_H

#include <sstream>
#include "Instruction.h"

using namespace std;

class AddSP: public ThumbInstruction
{
public:
    AddSP(int imm):ThumbInstruction(ADDSP, imm){}

    static AddSP* decode(int opcode){
        char imm = opcode&0x7F;
        int signedImm = (opcode&0x80)? -imm : imm;
        return new AddSP(signedImm);
    }

    string toString(){
        stringstream stream;
        stream << showbase << "ADD SP, " << hex << getImmediate() << " (" << dec << getImmediate() << ")";
        return stream.str();
    }
};

#endif
