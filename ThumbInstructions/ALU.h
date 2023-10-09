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

    static ThumbALU* decode(int opcode){
        char rS = (opcode>>3) & 0b111;
        char rD = opcode & 0b111;
        char op = (opcode>>6) & 0xF;
        switch (op)
        {
        case 0xF:
            return new ThumbALU(MVN, rD, rS);
        default:
            cout << "ThumbALU = " << unsigned(op) << endl;
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
        default:
            cout << "ThumbALU = " << hex << getOpcode() << endl;
            exit(FAILED_DECODED_TO_STRING);
        }
        return stream.str();
    }
};

#endif
