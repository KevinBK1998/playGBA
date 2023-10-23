#include <sstream>
#include "../ThumbCpu.h"

class HiRegOperation: public ThumbInstruction
{
private:
    char regSource;
public:
    HiRegOperation(char regS): ThumbInstruction(BX){
        regSource = regS;
    }
    HiRegOperation(Opcode op, char rD, char rS): ThumbInstruction(op, rD){
        regSource = rS;
    }

    char getRegSource(){
        return regSource;
    }

    static HiRegOperation* decode(int opcode){
        char op = (opcode>>8) & 0b11;
        char rS = (opcode>>3) & 0xF;
        char rD = opcode & 0b111;
        rD |= ((opcode>>4) & 0x8);
        switch (op)
        {
        case 1:
            return new HiRegOperation(CMP_HI, rD, rS);
        case 2:
            return new HiRegOperation(MOV_HI, rD, rS);
        case 3:
            return new HiRegOperation(rS);
        default:
            cout << "HiRegOperation = " << unsigned(op) << endl;
            exit(FAILED_TO_DECODE);
            break;
        }
    }

    string toString(){
        stringstream stream;
        switch (getOpcode())
        {
        case CMP_HI:
            stream<<"CMP";
            break;
        case MOV_HI:
            stream<<"MOV";
            break;
        case BX:
            stream<<"BX";
            break;
        default:
            cout<<"HiRegOperation = "<<getOpcode()<<endl;
            exit(FAILED_DECODED_TO_STRING);
        }
        if(getOpcode() != BX)
            stream<<" R"<<unsigned(getRegDest())<<",";
        stream<<" R"<<unsigned(regSource);
        return stream.str();
    }
};

void ThumbCpu::compareHigh(){
    HiRegOperation* alu = (HiRegOperation*) decodedInstruction;
    uint64_t before = reg->getReg(alu->getRegDest());
    if(alu->getRegDest() == PC)
        before+=HALFWORD_SIZE; //Source Register is PC+4
    uint32_t data = reg->getReg(alu->getRegSource());
    if(alu->getRegSource() == PC)
        data+=HALFWORD_SIZE; //Source Register is PC+4
    uint64_t result = before - data;
    DEBUG_OUT<<"result = "<< result << endl;
    reg->setFlags(NZCV, generateFlags(before, -data, result));
}

void ThumbCpu::moveHigh(){
    HiRegOperation* alu = (HiRegOperation*) decodedInstruction;
    int data = reg->getReg(alu->getRegSource());
    if(alu->getRegSource() == PC)
        data+=HALFWORD_SIZE; //Source Register is PC+4
    DEBUG_OUT<<"data = "<<data<<endl;
    reg->setReg(alu->getRegDest(), data);
}

void ThumbCpu::branchExchange(){
    HiRegOperation* b = (HiRegOperation*) decodedInstruction;
    int jumpTo = reg->getReg(b->getRegSource());
    if(b->getRegSource() == PC)
        jumpTo=(jumpTo + HALFWORD_SIZE) & ~2; //Source Register is (PC+4) AND NOT 2
    DEBUG_OUT<<"jumpAddress = "<<jumpTo<<endl;
    reg->exchange(jumpTo);
}
