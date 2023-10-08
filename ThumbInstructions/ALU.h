#ifndef THUMB_ALU_H
#define THUMB_ALU_H

#include <stdint.h>
#include "Instruction.h"
using namespace std;
class ThumbALU: public ThumbInstruction
{
private:
    char regSource;
public:
    ThumbALU(Opcode opcode, char regD, int imm):ThumbInstruction(opcode, regD, imm){}
    ThumbALU(Opcode opcode, char regD, char regS, int imm):ThumbInstruction(opcode, regD, imm){
        regSource = regS;
    }

    static ThumbALU* decode(int opcode){
        char op = (opcode>>9) & 0x3;
        uint8_t imm = (opcode>>6) & 0b111;
        char rS = (opcode>>3) & 0b111;
        char rD = opcode & 0b111;
        switch (op)
        {
        case 2:
            return new ThumbALU(ADD, rD, rS, imm);
        default:
            exit(FAILED_TO_DECODE);
            break;
        }
    }

    // decode for Immediate only ALU, bool value does not matter
    static ThumbALU* decode(int opcode, bool ImmediateOnly){
        char op = (opcode>>11) & 0x3;
        char rD = (opcode>>8) & 0xF;
        uint8_t imm = opcode & 0xFF;
        switch (op)
        {
        case 0:
            return new ThumbALU(MOV, rD, imm);
        default:
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
        case MOV:
            stream << showbase << "MOV R" << unsigned(getRegDest()) << hex << ", " << getImmediate();
            return stream.str();
        case ADD:
            stream << showbase << "ADD R" << unsigned(getRegDest())<<", R" << unsigned(regSource) << hex << ", " << getImmediate();
            return stream.str();
        default:
            cout << "ALU = " << hex << getOpcode() << endl;
            exit(FAILED_DECODED_TO_STRING);
        }
        return "Undefined";
    }
};

#endif
