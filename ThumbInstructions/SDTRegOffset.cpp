#include <sstream>
#include "../ThumbCpu.h"

class SDTRegOffset : public ThumbInstruction
{
private:
    char regBase;
    bool byteTransfer;
public:
    SDTRegOffset(Opcode opcode, bool flag, char rD, char rB, int offset): ThumbInstruction(opcode, rD, offset){
        byteTransfer = true;
        regBase = rB;
    }

    static SDTRegOffset* decode(int opcode){
        char op = (opcode>>11) & 0b11;
        int offset = (opcode>>6) & 0b11111;
        char rB = (opcode>>3) & 0b111;
        char rD = opcode & 0b111;
        switch (op)
        {
        case 2:
            return new SDTRegOffset(STR, true, rD, rB, offset);
        default:
            cout << "SDTRegOffset = " << opcode << endl;
            exit(FAILED_TO_DECODE);
            break;
        }
    }

    char getRegBase(){
        return regBase;
    }

    bool useImmediate(){
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
            stream<<"STR"<<(byteTransfer?"B":"");
            break;
        default:
            cout << "SDTRegOffset = " << hex << getOpcode() << endl;
            exit(FAILED_DECODED_TO_STRING);
        }
        stream<<" R" << unsigned(getRegDest()) <<", [R"<<unsigned(regBase);
        if(getImmediate())
            stream<<", "<<hex<<showbase<< getImmediate();
        stream<<"]";
        return stream.str();
    }
};

void ThumbCpu::storeImmediateOffset(){
    SDTRegOffset* sdt = (SDTRegOffset*) decodedInstruction;
    int base = reg->getReg(sdt->getRegBase());
    int offset = sdt->getImmediate();
    int address = base + offset;
    int data = reg->getReg(sdt->getRegDest());
    if (sdt->isByteTransfer()){
        cout<<"address = "<< address<<", data = "<< (data & 0xFF) << endl;
        mem->write8(address, data);
    }
    else{
        cout<<"address = "<< address<<", data = "<< data << endl;
        mem->write32(address, data);
    }
}
