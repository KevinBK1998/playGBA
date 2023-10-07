#include <stdint.h>
#include "../Opcode.h"
#include "Instruction.h"
using namespace std;
class ThumbALU: public ThumbInstruction
{
private:
    char regDest;
    int immediate;
public:
    ThumbALU(Opcode opcode, char regD, int imm):ThumbInstruction(opcode){
        regDest = regD;
        immediate = imm;
    }

    static ThumbALU* decodeALU(int opcode){
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

    char getRegDest(){
        return regDest;
    }

    int getImmediate(){
        return immediate;
    }

    string toString(){
        stringstream stream;
        switch (getOpcode())
        {
        case MOV:
            stream << showbase << "MOV"<<" R" << unsigned(regDest) << hex << ", " << immediate;
            return stream.str();
        default:
            cout << "ALU = " << hex << getOpcode() << endl;
            exit(FAILED_DECODED_TO_STRING);
        }
        return "Undefined";
    }
};
