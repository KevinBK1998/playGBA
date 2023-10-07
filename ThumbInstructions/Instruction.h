#ifndef THUMB_INSTR_H
#define THUMB_INSTR_H

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
    ThumbInstruction(Opcode, char, int);
    Opcode getOpcode();
    char getRegDest();
    int getImmediate();
    virtual string toString();
};

#endif