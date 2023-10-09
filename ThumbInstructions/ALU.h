#ifndef THUMB_ALU_H
#define THUMB_ALU_H

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
            stream << showbase << "MVN";
            break;
        case TST:
            stream << showbase << "TST";
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
    int flags = generateFlags(result);
    cout<<"result = "<<result<<", flags = "<<flags<<endl;
    reg->setFlags(flags);
}

void ThumbCpu::moveN(){
    ThumbALU* alu = (ThumbALU*) decodedInstruction;
    int data = reg->getReg(alu->getRegSource());
    data = ~data;
    int flags = generateFlags(data);
    cout<<"data = " << data <<", flags = "<< flags<< endl;
    reg->setReg(alu->getRegDest(), data);
    reg->setFlags(flags);
}

#endif
