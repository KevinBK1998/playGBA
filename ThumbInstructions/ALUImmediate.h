#include <sstream>
#include "../ThumbCpu.h"

class ALUThumbIMM: public ThumbInstruction
{
public:
    ALUThumbIMM(Opcode opcode, char regD, int imm):ThumbInstruction(opcode, regD, imm){}

    static ALUThumbIMM* decode(int opcode){
        uint8_t imm = opcode&0xFF;
        char op = (opcode>>11) & 0x3;
        char rD = (opcode>>8) & 0x7;
        switch (op)
        {
        case 0:
            return new ALUThumbIMM(MOV, rD, imm);
        case 3:
            return new ALUThumbIMM(SUB, rD, imm);
        default:
            cout << "ALUThumbIMM = " << opcode << endl;
            exit(FAILED_TO_DECODE);
            break;
        }
    }

    string toString(){
        stringstream stream;
        switch (getOpcode())
        {
        case MOV:
            stream<<"MOV";
            break;
        case SUB:
            stream<<"SUB";
            break;
        default:
            cout << "ALUThumbIMM = " << hex << getOpcode() << endl;
            exit(FAILED_DECODED_TO_STRING);
        }
        stream<<" R"<< unsigned(getRegDest()) << showbase <<  hex << ", " << getImmediate();
        return stream.str();
    }
};

void ThumbCpu::move(){
    int immediate = decodedInstruction->getImmediate();
    int flags = generateFlags(immediate);
    cout<<"result = "<< immediate <<", flags = "<< flags << endl;
    reg->setReg(decodedInstruction->getRegDest(), immediate);
    reg->setFlags(generateFlags(immediate));
}

void ThumbCpu::sub(){
    int immediate = decodedInstruction->getImmediate();
    int result = reg->getReg(decodedInstruction->getRegDest()) - immediate;
    int flags = generateFlags(result);
    cout<<"result = "<< result<<", flags = "<< flags << endl;
    reg->setReg(decodedInstruction->getRegDest(), result);
    reg->setFlags(generateFlags(immediate));
}
