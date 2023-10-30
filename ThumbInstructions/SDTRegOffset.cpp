#include <sstream>
#include "../ThumbCpu.h"

class SDTRegOffset : public ThumbInstruction
{
private:
    char regBase;
    bool byteTransfer;
public:
    SDTRegOffset(Opcode opcode, bool flag, char rD, char rB, int offset): ThumbInstruction(opcode, rD, offset){
        byteTransfer = flag;
        regBase = rB;
    }

    static SDTRegOffset* decode(int opcode){
        char op = (opcode>>11) & 0b11;
        int offset = (opcode>>6) & 0b11111;
        char rB = (opcode>>3) & 0b111;
        char rD = opcode & 0b111;
        switch (op)
        {
        case 0:
            return new SDTRegOffset(STR, false, rD, rB, offset);
        case 1:
            return new SDTRegOffset(LDR, false, rD, rB, offset);
        case 2:
            return new SDTRegOffset(STR, true, rD, rB, offset);
        case 3:
            return new SDTRegOffset(LDR, true, rD, rB, offset);
        default:
            cout << "SDTRegOffset = " << unsigned(op) << endl;
            exit(FAILED_TO_DECODE);
            break;
        }
    }

    char getRegBase(){
        return regBase;
    }

    bool useImmediateOffset(){
        return true;
    }

    bool isByteTransfer(){
        return byteTransfer;
    }

    string toString(){
        stringstream stream;
        switch (getOpcode())
        {
        case STR:
            stream<<"STR";
            break;
        case LDR:
            stream<<"LDR";
            break;
        default:
            cout << "SDTRegOffset = " << hex << getOpcode() << endl;
            exit(FAILED_DECODED_TO_STRING);
        }
        stream<<(byteTransfer?"B":"")<<" R" << unsigned(getRegDest()) <<", [R"<<unsigned(regBase);
        if(getImmediate())
            stream<<", "<<hex<<showbase<<(byteTransfer?1:4)*getImmediate();
        stream<<"]";
        return stream.str();
    }
};

void ThumbCpu::storeImmediateOffset(){
    SDTRegOffset* sdt = (SDTRegOffset*) decodedInstruction;
    int base = reg->getReg(sdt->getRegBase());
    int offset = sdt->getImmediate();
    if (!sdt->isByteTransfer())
        offset*=4;
    int address = base + offset;
    int data = reg->getReg(sdt->getRegDest());
    if (sdt->isByteTransfer()){
        DEBUG_OUT<<"address strb = "<< address<<", data = "<< (data & 0xFF) << endl;
        mem->write8(address, data);
    }
    else{
        DEBUG_OUT<<"address = "<< address<<", data = "<< data << endl;
        mem->write32(address, data);
    }
}

void ThumbCpu::loadImmediateOffset(){
    SDTRegOffset* sdt = (SDTRegOffset*) decodedInstruction;
    int base = reg->getReg(sdt->getRegBase());
    int offset = sdt->getImmediate();
    if (!sdt->isByteTransfer())
        offset*=4;
    int address = base + offset;
    int data;
    if (sdt->isByteTransfer()){
        data = mem->read8(address);
        DEBUG_OUT<<"address ldrb = "<< address<<", data = "<< (data & 0xFF) << endl;
    }
    else{
        data = mem->read32(address);
        DEBUG_OUT<<"address = "<< address<<", data = "<< data << endl;
    }
    reg->setReg(sdt->getRegDest(), data);
}
