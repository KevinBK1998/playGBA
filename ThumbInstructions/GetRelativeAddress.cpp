#include <sstream>
#include "../ThumbCpu.h"

class GetRelativeAddress: public ThumbInstruction
{
private:
    bool useSP;
public:
    GetRelativeAddress(bool isSP, char rD, int imm): ThumbInstruction(MOV_REG_ADDRESS, rD, imm){
        useSP = isSP;
    }

    static GetRelativeAddress* decode(int opcode){
        bool useSP = (opcode>>11) & 1;
        char rD = (opcode>>8) & 7;
        int offset = (opcode & 0xFF)<<2;
        return new GetRelativeAddress(useSP, rD, offset);
    }

    bool shouldUseSP(){
        return useSP;
    }

    string toString(){
        stringstream stream;
        stream <<"ADD R"<<unsigned(getRegDest())<<", ";
        if (useSP)
            stream << "SP";
        else
            stream << "PC";
        stream <<", "<<hex<<showbase<<getImmediate();
        return stream.str();
    }
};

void ThumbCpu::moveRegAddress(){
    GetRelativeAddress* mov = (GetRelativeAddress*) decodedInstruction;
    int op1;
    if (mov->shouldUseSP())
        op1 = reg->getReg(SP);
    else
        op1 = (reg->getPC() + 4) & ~2; // PC value should be (($+4) AND NOT 2)
    int op2 = mov->getImmediate();
    int result = op1 + op2;
    DEBUG_OUT<<"result = "<<result<<", data ="<< op1 << endl;
    reg->setReg(mov->getRegDest(), result);
}
