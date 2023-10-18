#include <sstream>
#include "../ThumbCpu.h"

class SDTRelativeSP : public ThumbInstruction
{
public:
    SDTRelativeSP(Opcode opcode, char rD, int imm): ThumbInstruction(opcode, rD, imm){}

    static SDTRelativeSP* decode(int opcode){
        Opcode operation;
        if((opcode>>11) & 0b1)
            operation=LDRSP;
        else
            operation=STRSP;
        char rD = (opcode>>8) & 0b111;
        int imm = opcode & 0xFF;
        return new SDTRelativeSP(operation, rD, imm);
    }

    string toString(){
        stringstream stream;
        switch (getOpcode())
        {
        case LDRSP:
            stream << "LDR";
            break;
        case STRSP:
            stream << "STR";
            break;
        default:
            cout << "SDTRelativeSP = " << hex << getOpcode() << endl;
            exit(FAILED_DECODED_TO_STRING);
        }
        stream<<" R" << unsigned(getRegDest()) <<", [SP";
        if(getImmediate())
            stream<<", "<<hex<<showbase<< getImmediate();
        stream<<"]";
        return stream.str();
    }
};

void ThumbCpu::loadRegSPRelative(){
    int regBValue = reg->getReg(SP);
    int address = regBValue + decodedInstruction->getImmediate()*4;
    int data = mem->read32(address);
    DEBUG_OUT<<"address = "<< address <<", data = "<< data << endl;
    reg->setReg(decodedInstruction->getRegDest(), data);
}

void ThumbCpu::storeRegSPRelative(){
    int regBValue = reg->getReg(SP);
    int address = regBValue + decodedInstruction->getImmediate()*4;
    int data = reg->getReg(decodedInstruction->getRegDest());
    DEBUG_OUT<<"address = "<< address <<", data = "<< data << endl;
    mem->write32(address, data);
}
