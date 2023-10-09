#include <sstream>
#include "../ThumbCpu.h"

class ThumbBranch: public ThumbInstruction
{
public:
    ThumbBranch(int imm): ThumbInstruction(B, imm){}

    static ThumbBranch* decode(int opcode){
        int imm = opcode & 0x7FF;
        imm |= (imm&0x400)? 0xFFFFF800: 0;
        return new ThumbBranch(imm);
    }

    string toString(){
        stringstream stream;
        stream<<"B "<<showbase<<hex<<getImmediate();
        return stream.str();
    }
};

void ThumbCpu::branch(){
    reg->branch(decodedInstruction->getImmediate());
}
