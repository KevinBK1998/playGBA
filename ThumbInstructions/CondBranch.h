#include "../ThumbCpu.h"

using namespace std;

class CondBranch: public ThumbInstruction
{
private:
    Condition condition;
public:
    CondBranch(Condition cond, int imm): ThumbInstruction(B_COND, imm){
        condition = cond;
    }

    static CondBranch* decode(int opcode){
        Condition cond = Condition((opcode>>8) & 0xF);
        int imm = opcode & 0xFF;
        if (imm & 0x80)
            imm |= 0xFFFFFF00;
        return new CondBranch(cond, imm);
    }

    Condition getPreCheck(){
        return condition;
    }

    string toString(){
        stringstream stream;
        stream<<"B"<<condition.toString()<<showbase<<hex<<" "<<getImmediate()<<" ("<<dec<<getImmediate()<<")";
        return stream.str();
    }
};

void ThumbCpu::condBranch(){
    CondBranch* b = (CondBranch*) decodedInstruction;
    if (canExecute(b->getPreCheck())){
        reg->branch(b->getImmediate());
    } else DEBUG_OUT << "Skipping, condition failed" << endl;
}
