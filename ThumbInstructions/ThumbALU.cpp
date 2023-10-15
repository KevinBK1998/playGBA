#include <sstream>
#include "../ThumbCpu.h"

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
        case 8:
            return new ThumbALU(TST, rD, rS);
        case 0xA:
            return new ThumbALU(CMP, rD, rS);
        case 0xC:
            return new ThumbALU(ORR, rD, rS);
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
        case TST:
            stream<<"TST";
            break;
        case CMP:
            stream<<"CMP";
            break;
        case ORR:
            stream<<"ORR";
            break;
        case MVN:
            stream<<"MVN";
            break;
        default:
            cout << "ThumbALU = " << hex << getOpcode() << endl;
            exit(FAILED_DECODED_TO_STRING);
        }
        stream<<" R"<<unsigned(getRegDest())<<", R"<<unsigned(regSource);
        return stream.str();
    }
};

void ThumbCpu::test(){
    ThumbALU* alu = (ThumbALU*) decodedInstruction;
    int op1 = reg->getReg(alu->getRegSource());
    int op2 = reg->getReg(alu->getRegDest());
    int result = op1 & op2;
    DEBUG_OUT<<"result = "<<result<<endl;
    reg->setFlags(NZ, generateFlags(result));
}

void ThumbCpu::compareReg(){
    ThumbALU* alu = (ThumbALU*) decodedInstruction;
    uint64_t op1 = reg->getReg(alu->getRegDest());
    uint32_t op2 = reg->getReg(alu->getRegSource());
    uint64_t result = op1 - op2;
    DEBUG_OUT<<"result = "<<result<<endl;
    reg->setFlags(NZCV, generateFlags(op1, -op2, result));
}

void ThumbCpu::logicalOR(){
    ThumbALU* alu = (ThumbALU*) decodedInstruction;
    int op1 = reg->getReg(alu->getRegSource());
    int op2 = reg->getReg(alu->getRegDest());
    int result = op1 | op2;
    DEBUG_OUT<<"result = "<<result<<endl;
    reg->setReg(alu->getRegDest(), result);
    reg->setFlags(NZ, generateFlags(result));
}

void ThumbCpu::moveN(){
    ThumbALU* alu = (ThumbALU*) decodedInstruction;
    int data = reg->getReg(alu->getRegSource());
    data = ~data;
    DEBUG_OUT<<"data = " << data << endl;
    reg->setReg(alu->getRegDest(), data);
    reg->setFlags(NZ, generateFlags(data));
}
