#include <sstream>
#include "../ArmCpu.h"

class Multiply : public ArmInstruction
{
private:
    bool updatePSR;
    char regM;
    char regS;
public:
    Multiply(Opcode opcode, Condition cond, bool psr, char rD, char rM, char rS):ArmInstruction(cond, opcode, 0, rD){
        updatePSR = psr;
        regM = rM;
        regS = rS;
    }

    static Multiply* decode(int opcode){
        Condition cond = Condition((opcode >> 28) & 0xF);
        char op = (opcode>>21) & 0xF;
        bool psr = (opcode>>20) & 1;
        char rD = (opcode>>16) & 0xF;
        char rN = (opcode>>12) & 0xF;
        char rS = (opcode>>8) & 0xF;
        int flags = (opcode>>4) & 0xF;
        char rM = opcode & 0xF;
        switch (op)
        {
        case 0:
            return new Multiply(MUL, cond, psr, rD, rM, rS);
            // return new Multiply(MUL, cond, psr, rD, rN, rS, flags, rM);
        default:
            cout << "Multiply = " << unsigned(op) << endl;
            exit(FAILED_TO_DECODE);
        }
    }

    char getRegM(){
        return regM;
    }

    char getRegS(){
        return regS;
    }

    bool shouldUpdatePSR(){
        return updatePSR;
    }

    string toString(){
        stringstream stream;
        switch (getOpcode())
        {
        case MUL:
            stream << "MUL";
            break;
        default:
            exit(FAILED_DECODED_TO_STRING);
        }
        stream<< getCondition() <<" R" << getRegDest() << ", R"<< unsigned(regM)<< ", R"<< unsigned(regS);
        return stream.str();
    }
};

void ArmCpu::multiply(){
    Multiply* m = (Multiply*) decodedInstruction;
    int op1 = reg->getReg(m->getRegM());
    int op2 = reg->getReg(m->getRegS());
    int result = op1*op2;
    DEBUG_OUT<<"result = "<< result << endl;
    reg->setReg(m->getRegDest(), result);
    if (m->shouldUpdatePSR())
        reg->setFlags(NZ, generateFlags(result));
}
