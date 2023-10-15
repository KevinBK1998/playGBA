#include <sstream>
#include "../ThumbCpu.h"

using namespace std;

class ExtendedSDT : public ThumbInstruction
{
private:
    char regBase;
    char regOffset;
public:
    ExtendedSDT(Opcode opcode, char rO, char rB, char rD): ThumbInstruction(opcode, rD){
        regBase = rB;
        regOffset = rO;
    }

    static ExtendedSDT* decode(int opcode){
        char op = (opcode>>10) & 0b11;
        char rO = (opcode>>6) & 0b111;
        char rB = (opcode>>3) & 0b111;
        char rD = opcode & 0b111;
        switch (op)
        {
        case 2:
            return new ExtendedSDT(LDRH_E, rO, rB, rD);
        default:
            cout << "ExtendedSDT = " << hex << unsigned(op) << endl;
            exit(FAILED_TO_DECODE);
            break;
        }
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
        case LDRH_E:
            stream << showbase << "LDRH R" << unsigned(getRegDest()) <<", [R"<<unsigned(regBase) << ", R" << unsigned(regOffset)<<"]";
            return stream.str();
        default:
            cout << "ExtendedSDT = " << hex << getOpcode() << endl;
            exit(FAILED_DECODED_TO_STRING);
        }
        return "Undefined";
    }
};

void ThumbCpu::loadHalfRegExtended(){
    ExtendedSDT* sdt = (ExtendedSDT*) decodedInstruction;
    int base = reg->getReg(sdt->getRegBase());
    DEBUG_OUT<<"address = "<< base << endl;
    int offset = reg->getReg(sdt->getRegOffset());
    int address = base + offset;
    int data = mem->read16(address);
    DEBUG_OUT<<"address = "<< address <<", data = "<< data << endl;
    reg->setReg(sdt->getRegDest(), data);
}
