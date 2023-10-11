#include <sstream>
#include "../ArmCpu.h"

using namespace std;

class PSRTransfer: public ArmInstruction
{
private:
    bool useSPSR;
    char mask;
    char regSource;

    string PSRToString(){
        stringstream stream;
        if (useSPSR)
            stream<<"S";
        else
            stream<<"C";
        stream<<"PSR";
        return stream.str();
    }

    string getMaskString(){
        stringstream stream;
        if ((mask >> 3) & 1)
            stream << "f";
        if ((mask >> 2) & 1)
            stream << "s";
        if ((mask >> 1) & 1)
            stream << "x";
        if (mask & 1)
            stream << "c";
        return stream.str();
    }

public:
    PSRTransfer(Opcode opcode, Condition cond, char destReg, bool psr): ArmInstruction(cond, opcode, 0, destReg){ // TODO remove dummy immediate here
        useSPSR = psr;
    }

    PSRTransfer(Opcode opcode, Condition cond, bool psr, char m, char rS): ArmInstruction(cond, opcode){
        useSPSR = psr;
        mask = m;
        regSource = rS;
    }

    PSRTransfer(Opcode opcode, Condition cond, bool psr, char m, uint8_t imm): ArmInstruction(cond, opcode, imm){
        useSPSR = psr;
        mask = m;
    }

    char getRegSource(){
        return regSource;
    }

    bool shouldUseSPSR(){
        return useSPSR;
    }

    int getExpandedMask(){
        int expMask = 0;
        for (int i = 3; i >= 0; i--) {
            expMask = expMask << 8;
            if ((mask >> i) & 1)
                expMask += 0xFF;
        }
        return expMask;
    }

    static PSRTransfer* decode(int opcode){
        Condition cond = Condition((opcode >> 28) & 0xF);
        bool useImm = (opcode >> 25) & 1;
        bool psr = (opcode >> 22) & 1;
        char mask = (opcode >> 16) & 0xF;
        char rDest = (opcode >> 12) & 0xF;
        if(!((opcode >> 21) & 1)) {
            if (mask == 0xF)
                return new PSRTransfer(MRS, cond, rDest, psr);
            cout << "PSRTransfer (SWP) = " << opcode << endl;
            exit(FAILED_TO_DECODE);
        }
        else{
            if (!useImm){
                if (!((opcode>>4) & 0xFF)){
                    char rSource = opcode & 0xF;
                    return new PSRTransfer(MSR, cond, psr, mask, rSource);
                }
                cout << "PSRTransfer (BX) = " << opcode << endl;
                exit(FAILED_TO_DECODE);
            }
            else
            {
                int shift = (opcode>>8) & 0xF;
                uint8_t imm = opcode & 0xFF;
                cout << "PSRTransfer (ROR imm) = " << opcode << endl;
                exit(FAILED_TO_DECODE);
                return new PSRTransfer(MSR, cond, psr, mask, imm);    
            }
        }
    }

    string toString(){
        stringstream stream;
        switch (getOpcode())
        {
        case MRS:
            stream<<"MRS"<<getCondition()<<" R"<<getRegDest()<<", "<< PSRToString();
            break;
        case MSR:
            stream<<"MSR"<<getCondition()<<" "<<PSRToString()<<"_"<<getMaskString()<<", R"<<unsigned(regSource);
            break;
        default:
            cout << "PSRTransfer = " << hex << getOpcode() << endl;
            exit(FAILED_DECODED_TO_STRING);
        }
        return stream.str();
    }
};

void ArmCpu::psrTransfer(){
    PSRTransfer* p = (PSRTransfer*) decodedInstruction;
    if (decodedInstruction->getOpcode() == MRS){
        exit(FAILED_TO_EXECUTE);
        int data = p->shouldUseSPSR() ? reg->getSPSR(): reg->getCPSR();
        cout<<"result = "<< data << endl;
        reg->setReg(p->getRegDest(), data);
    }
    else{
        int data = reg->getReg(p->getRegSource());
        int mask = p->getExpandedMask();
        int result = data & mask;
        cout<<"fmask = "<<mask<<"\tresult = "<<result<<endl;
        if(p->shouldUseSPSR())
            reg->setSPSR(result);
        else
            reg->setCPSR(result);
    }
}
