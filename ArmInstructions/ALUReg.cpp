#include <sstream>
#include "../ArmCpu.h"

class ALUReg: public ArmInstruction
{
private:
    bool updatePSR;
    char regM;
    ShiftType shiftType;
public:
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
        case 0x4:
            return new ALUReg(ADD, cond, psr, rDest, rN, rM, shiftType, imm);
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

    int getShiftedData(int data){
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

    string toString(){
        stringstream stream;
        switch (getOpcode())
        {
        case ADD:
            stream<<"ADD"<<(updatePSR?"S":"")<<getCondition()<<" R"<<getRegDest()<<", R"<< getRegN();
            break;
        case MOV:
            stream<<"MOV"<<(updatePSR?"S":"")<<getCondition()<<" R"<<getRegDest();
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
    int op1 = reg->getReg(alu->getRegN());
    int op2 = reg->getReg(alu->getRegM());
    op2 = alu->getShiftedData(op2);
    int result = op1 + op2;
    cout<<"result = "<< hex << result << endl;
    reg->setReg(decodedInstruction->getRegDest(), result);
    if (alu->shouldUpdatePSR())
        reg->setFlags(generateFlags(result));
}

void ArmCpu::moveShifted(){
    ALUReg* alu = (ALUReg*) decodedInstruction;
    int data = reg->getReg(alu->getRegM());
    data = alu->getShiftedData(data);
    cout<<"data = " << data << endl;
    reg->setReg(alu->getRegDest(), data);
    if (alu->shouldUpdatePSR())
        reg->setFlags(generateFlags(data));
}
