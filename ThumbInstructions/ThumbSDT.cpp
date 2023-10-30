#include <sstream>
#include "../ThumbCpu.h"

using namespace std;

class ThumbSDT : public ThumbInstruction
{
private:
    bool byteTransfer;
    char regBase;
    char regOffset;
public:
    ThumbSDT(Opcode opcode, bool flag, char rO, char rB, char rD): ThumbInstruction(opcode, rD){
        byteTransfer = flag;
        regBase = rB;
        regOffset = rO;
    }

    static ThumbSDT* decode(int opcode){
        char op = (opcode>>10) & 0b11;
        char rO = (opcode>>6) & 0b111;
        char rB = (opcode>>3) & 0b111;
        char rD = opcode & 0b111;
        switch (op)
        {
        case 0:
            return new ThumbSDT(STR, false, rO, rB, rD);
        case 2:
            return new ThumbSDT(STR, true, rO, rB, rD);
        default:
            cout << "ThumbSDT = " << hex << unsigned(op) << endl;
            exit(FAILED_TO_DECODE);
            break;
        }
    }

    bool isByteTransfer(){
        return byteTransfer;
    }

    char getRegBase(){
        return regBase;
    }

    char getRegOffset(){
        return regOffset;
    }

    string toString(){
        stringstream stream;
        switch (getOpcode())
        {
        case STR:
            stream << "STR";
            break;
        default:
            cout << "ThumbSDT = " << hex << getOpcode() << endl;
            exit(FAILED_DECODED_TO_STRING);
        }
        stream <<(byteTransfer?"B ":" ")<< showbase<< "R" << unsigned(getRegDest()) <<", [R"<<unsigned(regBase) << ", R" << unsigned(regOffset)<<"]";
        return stream.str();
    }
};

void ThumbCpu::storeReg(){
    ThumbSDT* sdt = (ThumbSDT*) decodedInstruction;
    int regBValue = reg->getReg(sdt->getRegBase());
    int regOValue = reg->getReg(sdt->getRegOffset());
    int data = reg->getReg(sdt->getRegDest());
    int address = regBValue + regOValue;
    DEBUG_OUT<<"address = "<< address <<", data = "<< data << endl;
    if (sdt->isByteTransfer())
        mem->write8(address, data);
    else
        mem->write32(address, data);
}
