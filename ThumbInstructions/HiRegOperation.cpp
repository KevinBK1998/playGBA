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
        rD |= ((opcode>>4) & 0x80);
        switch (op)
        {
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

void ThumbCpu::branchExchange(){
    HiRegOperation* b = (HiRegOperation*) decodedInstruction;
    int jumpTo = reg->getReg(b->getRegSource());
    cout<<"jumpAddress = "<<jumpTo<<endl;
    reg->exchange(jumpTo);
}

void ThumbCpu::moveHigh(){
    HiRegOperation* b = (HiRegOperation*) decodedInstruction;
    int data = reg->getReg(b->getRegSource());
    cout<<"data = "<<data<<endl;
    reg->setReg(b->getRegDest(), data);
}
