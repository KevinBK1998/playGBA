#include <stdint.h>
#include "../Opcode.h"
#include "Instruction.h"
using namespace std;
class ThumbALU: public ThumbInstruction
{
public:
    ThumbALU(Opcode opcode, char regD, int imm):ThumbInstruction(opcode, regD, imm){}

    static ThumbALU* decode(int opcode){
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

    string toString(){
        stringstream stream;
        switch (getOpcode())
        {
        case MOV:
            stream << showbase << "MOV"<<" R" << unsigned(getRegDest()) << hex << ", " << getImmediate();
            return stream.str();
        default:
            cout << "ALU = " << hex << getOpcode() << endl;
            exit(FAILED_DECODED_TO_STRING);
        }
        return "Undefined";
    }
};
