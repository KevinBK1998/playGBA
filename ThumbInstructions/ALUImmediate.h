#ifndef THUMB_ALU_IMM_H
#define THUMB_ALU_IMM_H

#include <iostream>
#include <sstream>
#include "Instruction.h"
#include "../FailureCodes.h"

using namespace std;

class ALUThumbIMM: public ThumbInstruction
{
public:
    ALUThumbIMM(Opcode opcode, char regD, int imm):ThumbInstruction(opcode, regD, imm){}

    static ALUThumbIMM* decode(int opcode){
        uint8_t imm = opcode&0xFF;
        char op = (opcode>>11) & 0x3;
        char rD = (opcode>>8) & 0xF;
        switch (op)
        {
        case 0:
            return new ALUThumbIMM(MOV, rD, imm);
        default:
            exit(FAILED_TO_DECODE);
            break;
        }
    }

    string toString(){
        stringstream stream;
        switch (getOpcode())
        {
        case MOV:
            stream << showbase << "MOV R" << unsigned(getRegDest()) << hex << ", " << getImmediate();
            break;
        default:
            cout << "ALUThumbIMM = " << hex << getOpcode() << endl;
            exit(FAILED_DECODED_TO_STRING);
        }
        return stream.str();
    }
};

#endif
