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
        case 1:
            return new ALUThumbIMM(CMP, rD, imm);
        case 2:
            return new ALUThumbIMM(ADD, rD, imm);
        case 3:
            return new ALUThumbIMM(SUB, rD, imm);
        default:
            cout << "ALUThumbIMM = " << opcode << " "<<unsigned(op)<< endl;
            exit(FAILED_TO_DECODE);
            break;
        }
    }

    bool useImmediate(){
        return true;
    }

    string toString(){
        stringstream stream;
        switch (getOpcode())
        {
        case MOV:
            stream<<"MOV";
            break;
        case CMP:
            stream<<"CMP";
            break;
        case ADD:
            stream<<"ADD";
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
    DEBUG_OUT<<"result = "<< immediate << endl;
    reg->setReg(decodedInstruction->getRegDest(), immediate);
    reg->setFlags(NZ, generateFlags(immediate));
}

void ThumbCpu::compare(){
    uint64_t before = reg->getReg(decodedInstruction->getRegDest());
    uint32_t immediate = decodedInstruction->getImmediate();
    uint64_t result = before - immediate;
    DEBUG_OUT<<"result = "<< result << endl;
    reg->setFlags(NZCV, generateFlags(before, -immediate, result));
}

void ThumbCpu::addImmediate(){
    uint64_t before = reg->getReg(decodedInstruction->getRegDest());
    uint32_t immediate = decodedInstruction->getImmediate();
    uint64_t result = before + immediate;
    DEBUG_OUT<<"result = "<< result << endl;
    reg->setReg(decodedInstruction->getRegDest(), result);
    reg->setFlags(NZCV, generateFlags(before, immediate, result));
}

void ThumbCpu::sub(){
    uint64_t before = reg->getReg(decodedInstruction->getRegDest());
    uint32_t immediate = decodedInstruction->getImmediate();
    uint64_t result = before - immediate;
    DEBUG_OUT<<"result = "<< result << endl;
    reg->setReg(decodedInstruction->getRegDest(), result);
    reg->setFlags(NZCV, generateFlags(before, -immediate, result));
}
