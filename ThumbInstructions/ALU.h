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
    ThumbALU(Opcode opcode, char regD, char regS):ThumbInstruction(opcode, regD){
        regSource = regS;
    }
    ThumbALU(Opcode opcode, char regD, char regS, int imm):ThumbInstruction(opcode, regD, imm){
        regSource = regS;
    }

    static ThumbALU* decode(int opcode){
        char rS = (opcode>>3) & 0b111;
        char rD = opcode & 0b111;
        if (((opcode>>10) & 0x3F)== 0b010000){
            char op = (opcode>>6) & 0xF;
            switch (op)
            {
            case 0xF:
                return new ThumbALU(MVN, rD, rS);
            default:
                cout << "ALU = " << unsigned(op) << endl;
                exit(FAILED_TO_DECODE);
                break;
            }
        }
        char op = (opcode>>9) & 0x3;
        uint8_t imm = (opcode>>6) & 0b111;
        switch (op)
        {
        case 2:
            return new ThumbALU(ADD, rD, rS, imm);
        default:
            cout << "ALU_add/subtract = " << unsigned(op) << endl;
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
        case MVN:
            stream << showbase << "MVN R" << unsigned(getRegDest()) << ", R" << unsigned(regSource);
            break;
        case ADD:
            stream << showbase << "ADD R"<< unsigned(getRegDest())<<", R" << unsigned(regSource) << hex << ", " << getImmediate();
            break;
        default:
            cout << "ThumbALU = " << hex << getOpcode() << endl;
            exit(FAILED_DECODED_TO_STRING);
        }
        return stream.str();
    }
};

#endif
