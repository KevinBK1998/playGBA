#ifndef THUMB_INSTR_H
#define THUMB_INSTR_H

#include "../CommonDS.h"

class ThumbInstruction
{
private:
    Opcode opcode;
    char regDest;
    int immediate;
public:
    ThumbInstruction();
    ThumbInstruction(Opcode);
    ThumbInstruction(Opcode, char);
    ThumbInstruction(Opcode, int);
    ThumbInstruction(Opcode, char, int);
    Opcode getOpcode();
    char getRegDest();
    int getImmediate();
    virtual bool useImmediate();
    virtual string toString();
};

#endif