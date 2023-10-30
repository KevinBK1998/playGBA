#include "../ThumbCpu.h"

using namespace std;

class ShiftMove: public ThumbInstruction
{
private:
    char regSource;
public:
    ShiftMove(Opcode operation, char regD, char regS, int offset): ThumbInstruction(operation, regD, offset){
        regSource = regS;
    }

    static ShiftMove* decode(int opcode){
        int op = (opcode>>11)&0b11;
        int offset = (opcode>>6)&0x1F;
        char regS = (opcode>>3)&0b111;
        char regD = opcode&0b111;
        switch (op)
        {
        case 0:
            return new ShiftMove(LSL, regD, regS, offset);
        case 1:
            return new ShiftMove(LSR, regD, regS, offset);
        case 2:
            return new ShiftMove(ASR, regD, regS, offset);
        default:
            cout << "ShiftMove = " << unsigned(op) << endl;
            exit(FAILED_TO_DECODE);
            break;
        }
    }

    char getRegSource(){
        return regSource;
    }
    
    string toString(){
        stringstream stream;
        switch (getOpcode())
        {
        case LSL:
            stream<<"LSL";
            break;
        case LSR:
            stream<<"LSR";
            break;
        case ASR:
            stream<<"ASR";
            break;
        default:
            cout << "ShiftMove = " << hex << getOpcode() << endl;
            exit(FAILED_DECODED_TO_STRING);
        }
        stream<<" R"<<unsigned(getRegDest())<<", R"<<unsigned(regSource)<<showbase<<hex<<", "<<getImmediate();
        return stream.str();
    }
};

void ThumbCpu::shiftLeft(){
    ShiftMove* alu = (ShiftMove*) decodedInstruction;
    uint64_t regSValue = reg->getReg(alu->getRegSource());
    int imm = alu->getImmediate();
    uint64_t result = regSValue << imm;
    bool carry = ((result>>32)&1)!=0;
    DEBUG_OUT<<"result = "<<result<<endl;
    char mask = imm? NZ:NZC;
    reg->setReg(alu->getRegDest(), result);
    reg->setFlags(mask, generateShiftFlags(carry, result));
}

void ThumbCpu::shiftRight(){
    ShiftMove* alu = (ShiftMove*) decodedInstruction;
    uint32_t regSValue = reg->getReg(alu->getRegSource());
    int imm = alu->getImmediate();
    if (!imm) {
        cout << "LSR Shift is zero"<<endl;
        exit(PENDING_CODE);
    }
    uint32_t result = regSValue >> (imm-1);
    bool carry = (result&1)!=0;
    result >>= 1;
    DEBUG_OUT<<"result = "<<result<<endl;
    reg->setReg(alu->getRegDest(), result);
    reg->setFlags(NZC, generateShiftFlags(carry, result));
}

void ThumbCpu::arithmeticShiftRight(){
    ShiftMove* alu = (ShiftMove*) decodedInstruction;
    int regSValue = reg->getReg(alu->getRegSource());
    int imm = alu->getImmediate();
    if (!imm) {
        cout << "ASR Shift is zero"<<endl;
        exit(PENDING_CODE);
    }
    uint64_t result = regSValue >> (imm-1);
    bool carry = (result&1)!=0;
    result >>= 1;
    DEBUG_OUT<<"result = "<<result<<endl;
    reg->setReg(alu->getRegDest(), result);
    reg->setFlags(NZC, generateShiftFlags(carry, result));
}
