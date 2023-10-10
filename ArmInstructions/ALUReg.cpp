#include <sstream>
#include "../ArmCpu.h"

using namespace std;

class ALUReg: public ArmInstruction
{
private:
    char regM;
    char shiftType;
public:
    ALUReg(Opcode opcode, Condition cond, char rDest, char rM, char type, int imm): ArmInstruction(cond, opcode, imm, rDest){
        regM = rM;
        shiftType = type;
    }

    static ALUReg* decode(int opcode){
        Condition cond = Condition((opcode >> 28) & 0xF);
        char op = (opcode >> 21) & 0xF;
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
        case 0xD:
            return new ALUReg(MOV, cond, rDest, rM, shiftType, imm);
        default:
            cout << "ALUReg = " << unsigned(op) << endl;
            exit(FAILED_TO_DECODE);
            break;
        //FIX LSL AND LSR FIRST
        }
    }

    char getRegM(){
        return regM;
    }

    char getShiftType(){
        return shiftType;
    }

    string toString(){
        stringstream stream;
        switch (getOpcode())
        {
        case MOV:
            stream<<"MOV"<<getCondition()<<" R"<<getRegDest();
            break;
        default:
            cout << "ALUReg = " << getOpcode() << endl;
            exit(FAILED_DECODED_TO_STRING);
        }
        stream<<", R"<<unsigned(regM);
        switch(shiftType){
        case LeftShift:
            stream<<",LSL#"<<hex<<showbase<<getImmediate();
            break;
        default:
            cout << "ALUReg shifttype = " << unsigned(shiftType) << endl;
            exit(FAILED_DECODED_TO_STRING);
        }
        return stream.str();
    }
};

void ArmCpu::moveShifted(){
    ALUReg* alu = (ALUReg*) decodedInstruction;
    int data = reg->getReg(alu->getRegM()) & 0xFF;
    int shift = alu->getImmediate();
    if (alu->getShiftType()||!alu->getImmediate()) {
        cout<<"shifttype = " << unsigned(alu->getShiftType())<<", shift = " << alu->getImmediate() << endl;
        exit(FAILED_TO_EXECUTE);
    }
    data = data<<shift;
    reg->setReg(decodedInstruction->getRegDest(), data);
    cout<<"data = " << data<<", shift = " << alu->getImmediate() << endl;
    // if (alu.canChangePsr())
    //     reg->setCPSR(setFlags(immediate));
}
