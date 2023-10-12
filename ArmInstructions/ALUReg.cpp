#include <sstream>
#include "../ArmCpu.h"

class ALUReg: public ArmInstruction
{
private:
    bool updatePSR;
    char regM;
    ShiftType shiftType;

    string getPSRToString(){
        return updatePSR?"S":"";
    }
public:
    ALUReg(Opcode opcode, Condition cond, char rN, char rM, char type, int imm): ArmInstruction(cond, opcode, rN, imm){
        updatePSR = true;
        regM = rM;
        shiftType = static_cast<ShiftType>(type);
    }

    ALUReg(Opcode opcode, Condition cond, bool psr, char rDest, char rN, char rM, char type, int imm): ArmInstruction(cond, opcode, rN, rDest, imm){
        updatePSR = psr;
        regM = rM;
        shiftType = static_cast<ShiftType>(type);
    }

    ALUReg(Opcode opcode, Condition cond, bool psr, char rDest, char rM, char type, int imm): ArmInstruction(cond, opcode, imm, rDest){
        updatePSR = psr;
        regM = rM;
        shiftType = static_cast<ShiftType>(type);
    }

    static ALUReg* decode(int opcode){
        Condition cond = Condition((opcode >> 28) & 0xF);
        char op = (opcode >> 21) & 0xF;
        bool psr = (opcode>>20) & 1;
        char rN = (opcode >> 16) & 0xF;
        char rDest = (opcode >> 12) & 0xF;
        int imm = (opcode>>7) & 0x1F;
        bool useShiftByReg = (opcode>>4) & 1;
        char shiftType = (opcode>>5) & 0b11;
        char rM = opcode & 0xF;
        if (useShiftByReg){
            cout << "ALUReg shiftBYreg = " << useShiftByReg<< endl;
            exit(FAILED_TO_DECODE);
        }
        switch (op)
        {
        case 4:
            return new ALUReg(ADD, cond, psr, rDest, rN, rM, shiftType, imm);
        case 0xA:
            return new ALUReg(CMP, cond, rN, rM, shiftType, imm);
        case 0xD:
            return new ALUReg(MOV, cond, psr, rDest, rM, shiftType, imm);
        default:
            cout << "ALUReg = " << unsigned(op) << endl;
            exit(FAILED_TO_DECODE);
            break;
        }
    }

    char getRegM(){
        return regM;
    }

    ShiftType getShiftType(){
        return shiftType;
    }

    bool shouldUpdatePSR(){
        return updatePSR;
    }

    uint64_t getShiftedData(int data){
        if (shiftType && !getImmediate()) {
            cout<<"shifttype = " << shiftType<<", shift = 0" << endl;
            exit(FAILED_TO_EXECUTE);
        }
        uint32_t copy = data;
        switch(shiftType){
        case ShiftLeft:
            return data<<getImmediate();
        case ShiftRight:
            return copy>>getImmediate();
        case ArithmeticShiftRight:
            return data>>getImmediate();
        default:
            cout<<"ALUReg shifttype = " << shiftType <<", shift = " << getImmediate() << endl;
            exit(FAILED_TO_EXECUTE);
        }
    }

    bool getShiftedCarry(long data){
        if (shiftType && !getImmediate()) {
            cout<<"shifttype = " << shiftType<<", shift = 0" << endl;
            exit(FAILED_TO_EXECUTE);
        }
        long result;
        bool carry;
        uint32_t copy = data;
        switch(shiftType){
        case ShiftLeft:
            result = data<<getImmediate();
            return (result>>32)&1;
        case ShiftRight:
            result = copy>>(getImmediate()-1);
            return result&1;
        case ArithmeticShiftRight:
            result = data>>(getImmediate()-1);
            cout<< "result = "<<result<< ", data = "<< data<<", immediate = "<< getImmediate()<<", flag = "<< (result&1)<<endl;
            exit(FAILED_TO_EXECUTE);
            return result&1;
        default:
            cout<<"ALUReg shifttype = " << shiftType <<", shift = " << getImmediate() << endl;
            exit(FAILED_TO_EXECUTE);
        }
    }

    string toString(){
        stringstream stream;
        switch (getOpcode())
        {
        case ADD:
            stream<<"ADD"<<getPSRToString()<<getCondition()<<" R"<<getRegDest()<<", R"<< getRegN();
            break;
        case CMP:
            stream<<"CMP"<<getCondition()<<" R"<< getRegN();
            break;
        case MOV:
            stream<<"MOV"<<getPSRToString()<<getCondition()<<" R"<<getRegDest();
            break;
        default:
            cout << "ALUReg = " << getOpcode() << endl;
            exit(FAILED_DECODED_TO_STRING);
        }
        stream<<", R"<<unsigned(regM);
        switch(shiftType){
        case ShiftLeft:
            stream<<",LSL#"<<hex<<showbase<<getImmediate();
            break;
        case ShiftRight:
            stream<<",LSR#"<<hex<<showbase<<getImmediate();
            break;
        default:
            cout << "ALUReg shifttype = " << unsigned(shiftType) << endl;
            exit(FAILED_DECODED_TO_STRING);
        }
        return stream.str();
    }
};

void ArmCpu::addShifted(){
    ALUReg* alu = (ALUReg*) decodedInstruction;
    uint64_t op1 = reg->getReg(alu->getRegN());
    uint32_t op2 = reg->getReg(alu->getRegM());
    op2 = alu->getShiftedData(op2);
    uint64_t result = op1 + op2;
    DEBUG_OUT<<"result = "<< hex << result << endl;
    reg->setReg(decodedInstruction->getRegDest(), result);
    if (alu->shouldUpdatePSR())
        reg->setFlags(NZCV, generateFlags(op1, op2, result));
}

void ArmCpu::cmpShifted(){
    ALUReg* alu = (ALUReg*) decodedInstruction;
    uint64_t op1 = reg->getReg(alu->getRegN());
    uint32_t op2 = reg->getReg(alu->getRegM());
    op2 = alu->getShiftedData(op2);
    uint64_t result = op1 - op2;
    DEBUG_OUT<<"result = "<< hex << result << endl;
    reg->setFlags(NZCV, generateFlags(op1, -op2, result));
}

void ArmCpu::moveShifted(){
    ALUReg* alu = (ALUReg*) decodedInstruction;
    uint64_t data = reg->getReg(alu->getRegM());
    bool carry = alu->getShiftedCarry(data);
    data = alu->getShiftedData(data);
    DEBUG_OUT<<"data = " << data << endl;
    reg->setReg(alu->getRegDest(), data);
    char mask=NZC;
    if (!alu->getShiftType() && alu->getImmediate()) mask = NZ; //LSL#0 does not change carry flag
    if (alu->shouldUpdatePSR())
        reg->setFlags(mask, generateShiftFlags(carry, data));
}
