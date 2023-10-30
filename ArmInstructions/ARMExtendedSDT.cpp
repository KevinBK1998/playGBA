#include <sstream>
#include "../ArmCpu.h"

class ARMExtendedSDT : public ArmInstruction
{
private:
    bool preFlag;
    bool addFlag;
    bool immFlag;
    bool writeBackFlag;
    char regBase;
    char regOffset;
public:
    ARMExtendedSDT(Condition cond, Opcode opcode, int flags, char rB, char rD, int imm):ArmInstruction(cond, opcode, imm, rD){
        preFlag = ((flags >> 3) & 1) != 0;
        addFlag = ((flags >> 2) & 1) != 0;//U
        immFlag = ((flags>>1) & 1) != 0; //I
        if (!immFlag)
            regOffset = imm;
        writeBackFlag = (flags & 1) != 0; //W
        regBase = rB;
    }

    static ARMExtendedSDT* decode(int opcode){
        Condition cond = Condition((opcode >> 28) & 0xF);
        int flags = (opcode>>21) & 0xF;
        bool loadFlag = (opcode>>20) & 1;
        if (!loadFlag){
            cout << "ARMExtendedSDT L = " << loadFlag << endl;
            exit(PENDING_CODE);
        }
        char rB = (opcode>>16) & 0xF;
        char rD = (opcode>>12) & 0xF;
        int imm = (opcode>>4) & 0xF0;
        char op = (opcode >> 5) & 0b11;
        imm |= (opcode & 0xF);
        switch (op)
        {
        case 1:
            return new ARMExtendedSDT(cond, LDRH, flags, rB, rD, imm);
        case 3:
            return new ARMExtendedSDT(cond, LDRSH, flags, rB, rD, imm);
        default:
            cout << "ARMExtendedSDT = " << unsigned(op) << endl;
            exit(FAILED_TO_DECODE);
            break;
        }
    }

    char getRegBase(){
        return regBase;
    }

    string toString(){
        stringstream stream;
        switch (getOpcode())
        {
        case LDRH:
            stream << "LDRH";
            break;
        case LDRSH:
            stream << "LDRSH";
            break;
        default:
            exit(FAILED_DECODED_TO_STRING);
        }
        stream<< getCondition() <<" R" << getRegDest() << ", [R"<< unsigned(getRegBase());
        if (immFlag){
            if(getImmediate())
                stream << (preFlag?", ":"] ") << (addFlag?"":"-")<< showbase << hex << getImmediate();
        }
        else
            stream << (preFlag?", ":"] ") << (addFlag?"":"-")<<" R" << unsigned(regOffset);
        stream << (preFlag?"]":"")<<(writeBackFlag?"!":"");
        return stream.str();
    }
};

void ArmCpu::loadHalfReg(){
    ARMExtendedSDT* sdt = (ARMExtendedSDT*) decodedInstruction;
    int base = reg->getReg(sdt->getRegBase());
    int offset = sdt->getImmediate();
    int address = base + offset;
    int data = mem->read16(address);
    DEBUG_OUT<<"address = "<< address <<", data = "<< data << endl;
    reg->setReg(sdt->getRegDest(), data);
}

void ArmCpu::loadSignedHalfReg(){
    ARMExtendedSDT* sdt = (ARMExtendedSDT*) decodedInstruction;
    int base = reg->getReg(sdt->getRegBase());
    int offset = sdt->getImmediate();
    int address = base + offset;
    int16_t data = mem->read16(address);
    DEBUG_OUT<<"address = "<< address <<", data = "<< data << endl;
    if (data<0){
        cout<<"ldrsh data -ve = "<< data << endl;
        exit(PENDING_CODE);
    }
    reg->setReg(sdt->getRegDest(), data);
}
