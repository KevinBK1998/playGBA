#include "../ThumbCpu.h"

class SDTHalfImmediate : public ThumbInstruction
{
private:
    char regBase;
public:
    SDTHalfImmediate(Opcode opcode, int offset, char rB, char rD): ThumbInstruction(opcode, rD, offset){
        regBase = rB;
    }

    static SDTHalfImmediate* decode(int opcode){
        Opcode op = STRH;
        if((opcode>>11) & 1) 
            op = LDRH;
        int offset = (opcode>>5) & 0b111110;
        char rB = (opcode>>3) & 0b111;
        char rD = opcode & 0b111;
        return new SDTHalfImmediate(op, offset, rB, rD);
    }

    char getRegBase(){
        return regBase;
    }

    string toString(){
        stringstream stream;
        switch (getOpcode())
        {
        case STRH:
            stream << "STRH";
            break;
        case LDRH:
            stream << "LDRH";
            break;
        default:
            cout << "SDTHalfImmediate = " << hex << getOpcode() << endl;
            exit(FAILED_DECODED_TO_STRING);
        }
        stream<<" R" << unsigned(getRegDest()) <<", [R"<<unsigned(regBase);
        if(getImmediate())
            stream<<", "<<hex<<showbase<< getImmediate();
        stream<<"]";
        return stream.str();
    }
};

void ThumbCpu::storeHalfReg(){
    SDTHalfImmediate* sdt = (SDTHalfImmediate*) decodedInstruction;
    int base = reg->getReg(sdt->getRegBase());
    int offset = sdt->getImmediate();
    int data = reg->getReg(sdt->getRegDest());
    int address = base + offset;
    DEBUG_OUT<<"address = "<< address <<", data = "<< data << endl;
    mem->write16(address, data);
}

void ThumbCpu::loadHalfReg(){
    SDTHalfImmediate* sdt = (SDTHalfImmediate*) decodedInstruction;
    int base = reg->getReg(sdt->getRegBase());
    int offset = sdt->getImmediate();
    int address = base + offset;
    int data = mem->read16(address);
    DEBUG_OUT<<"address = "<< address <<", data = "<< data << endl;
    reg->setReg(sdt->getRegDest(), data);
}
