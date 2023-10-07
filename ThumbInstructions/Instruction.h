#ifndef THUMB_INSTR_H
#define THUMB_INSTR_H

class ThumbInstruction
{
private:
    Opcode opcode;
public:
    ThumbInstruction();
    ThumbInstruction(Opcode);
    Opcode getOpcode();
    virtual string toString();
};

#endif