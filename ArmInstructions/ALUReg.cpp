#include <sstream>
#include "../ArmCpu.h"

class ALUReg: public ArmInstruction
{
private:
    bool updatePSR;
    bool useShiftByReg;
    char regM;
    char regShift;
    ShiftType shiftType;

    string getPSRToString(){
        return updatePSR?"S":"";
    }
public:
    ALUReg(Opcode opcode, Condition cond, char rN, char rM, bool useReg, char type, int imm): ArmInstruction(cond, opcode, rN, imm){
        updatePSR = true;
        regM = rM;
        useShiftByReg = useReg;
        if (useReg)
            regShift = imm>>1;
        shiftType = static_cast<ShiftType>(type);
    }

    ALUReg(Opcode opcode, Condition cond, bool psr, char rDest, char rN, char rM, bool useReg, char type, int imm): ArmInstruction(cond, opcode, rN, rDest, imm){
        updatePSR = psr;
        regM = rM;
        useShiftByReg = useReg;
        if (useReg)
            regShift = imm>>1;
        shiftType = static_cast<ShiftType>(type);
    }

    ALUReg(Opcode opcode, Condition cond, bool psr, char rDest, char rM, bool useReg, char type, int imm): ArmInstruction(cond, opcode, imm, rDest){
        updatePSR = psr;
        regM = rM;
        useShiftByReg = useReg;
        if (useReg)
            regShift = imm>>1;
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
        switch (op)
        {
        case 0:
            return new ALUReg(AND, cond, psr, rDest, rN, rM, useShiftByReg, shiftType, imm);
        case 2:
            return new ALUReg(SUB, cond, psr, rDest, rN, rM, useShiftByReg, shiftType, imm);
        case 4:
            return new ALUReg(ADD, cond, psr, rDest, rN, rM, useShiftByReg,shiftType, imm);
        case 0xA:
            return new ALUReg(CMP, cond, rN, rM, useShiftByReg, shiftType, imm);
        case 0xC:
            return new ALUReg(ORR, cond, psr, rDest, rN, rM, useShiftByReg, shiftType, imm);
        case 0xD:
            return new ALUReg(MOV, cond, psr, rDest, rM, useShiftByReg, shiftType, imm);
        case 0xE:
            return new ALUReg(BIC, cond, psr, rDest, rN, rM, useShiftByReg, shiftType, imm);
        default:
            cout << "ALUReg = " << unsigned(op) << endl;
            exit(FAILED_TO_DECODE);
            break;
        }
    }

    char getRegM(){
        return regM;
    }

    char getRegShift(){
        return regShift;
    }

    ShiftType getShiftType(){
        return shiftType;
    }

    bool shouldUpdatePSR(){
        return updatePSR;
    }

    char shouldUseShiftByReg(){
        return useShiftByReg;
    }

    bool getShiftedCarry(long data, int shift){
        if (shiftType && !shift) {
            cout<<"shifttype = " << shiftType<<", shift = 0" << endl;
            exit(FAILED_TO_EXECUTE);
        }
        long result;
        bool carry;
        uint32_t copy = data;
        switch(shiftType){
        case ShiftLeft:
            result = data<<shift;
            return (result>>32)&1;
        case ShiftRight:
            result = copy>>(shift-1);
            return result&1;
        case ArithmeticShiftRight:
            result = data>>(shift-1);
            cout<< "result = "<<result<< ", data = "<< data<<", immediate = "<< shift<<", flag = "<< (result&1)<<endl;
            exit(FAILED_TO_EXECUTE);
            return result&1;
        default:
            cout<<"ALUReg shifttype = " << shiftType <<", shift = " << shift << endl;
            exit(FAILED_TO_EXECUTE);
        }
    }

    bool getShiftedCarry(long data){
        return getShiftedCarry(data, getImmediate());
    }

    uint64_t getShiftedData(int data, int shift){
        DEBUG_OUT<<"shift = " << shift << endl;
        if (shiftType && !shift) {
            cout<<"shifttype = " << shiftType<<", shift = 0" << endl;
            exit(FAILED_TO_EXECUTE);
        }
        uint32_t copy = data;
        switch(shiftType){
        case ShiftLeft:
            return data<<shift;
        case ShiftRight:
            return copy>>shift;
        case ArithmeticShiftRight:
            return data>>shift;
        default:
            cout<<"ALUReg shifttype = " << shiftType <<", shift = " << shift << endl;
            exit(FAILED_TO_EXECUTE);
        }
    }

    uint64_t getShiftedData(int data){
        return getShiftedData(data, getImmediate());
    }

    string toString(){
        stringstream stream;
        switch (getOpcode())
        {
        case AND:
            stream<<"AND"<<getPSRToString()<<getCondition()<<" R"<<getRegDest()<<", R"<< getRegN();
            break;
        case ORR:
            stream<<"ORR"<<getPSRToString()<<getCondition()<<" R"<<getRegDest()<<", R"<< getRegN();
            break;
        case SUB:
            stream<<"SUB"<<getPSRToString()<<getCondition()<<" R"<<getRegDest()<<", R"<< getRegN();
            break;
        case ADD:
            stream<<"ADD"<<getPSRToString()<<getCondition()<<" R"<<getRegDest()<<", R"<< getRegN();
            break;
        case CMP:
            stream<<"CMP"<<getCondition()<<" R"<< getRegN();
            break;
        case MOV:
            stream<<"MOV"<<getPSRToString()<<getCondition()<<" R"<<getRegDest();
            break;
        case BIC:
            stream<<"BIC"<<getPSRToString()<<getCondition()<<" R"<<getRegDest()<<", R"<< getRegN();
            break;
        default:
            cout << "ALUReg = " << getOpcode() << endl;
            exit(FAILED_DECODED_TO_STRING);
        }
        stream<<", R"<<unsigned(regM);
        switch(shiftType){
        case ShiftLeft:
            stream<<",LSL";
            break;
        case ShiftRight:
            stream<<",LSR";
            break;
        case ArithmeticShiftRight:
            stream<<",ASR";
            break;
        default:
            cout << "ALUReg shifttype = " << unsigned(shiftType) << endl;
            exit(FAILED_DECODED_TO_STRING);
        }
        if(useShiftByReg)
            stream<<",R"<<unsigned(getRegShift());
        else
            stream<<"#"<<hex<<showbase<<getImmediate();
        return stream.str();
    }
};

void ArmCpu::andShifted(){
    ALUReg* alu = (ALUReg*) decodedInstruction;
    uint32_t op1 = reg->getReg(alu->getRegN());
    if (alu->getRegN()==PC)
        op1+=2*WORD_SIZE; //Rn is PC+12 for shift by reg
    uint32_t op2 = reg->getReg(alu->getRegM());
    if (alu->getRegM()==PC)
        op2+=2*WORD_SIZE; //Rm is PC+12 for shift by reg
    bool carry;
    if(alu->shouldUseShiftByReg()){
        int shift = reg->getReg(alu->getRegShift()) & 0xFF;
        carry = alu->getShiftedCarry(op2, shift);
        op2 = alu->getShiftedData(op2, shift);
    } else{
        carry = alu->getShiftedCarry(op2);
        op2 = alu->getShiftedData(op2);
    }
    uint32_t result = op1 & op2;
    DEBUG_OUT<<"result = " << result<<", carry = "<< carry << endl;
    reg->setReg(decodedInstruction->getRegDest(), result);
    char mask=NZC;
    if (!alu->getShiftType() && !alu->getImmediate()) mask = NZ; //LSL#0 does not change carry flag
    if (alu->shouldUpdatePSR())
        reg->setFlags(mask, generateShiftFlags(carry, result));
}

void ArmCpu::addShifted(){
    ALUReg* alu = (ALUReg*) decodedInstruction;
    uint64_t op1 = reg->getReg(alu->getRegN());
    if (alu->getRegN()==PC)
        op1+=2*WORD_SIZE; //Rn is PC+12 for shift by reg
    uint32_t op2 = reg->getReg(alu->getRegM());
    if (alu->getRegM()==PC)
        op2+=2*WORD_SIZE; //Rm is PC+12 for shift by reg
    if(alu->shouldUseShiftByReg()){
        int shift = reg->getReg(alu->getRegShift()) & 0xFF;
        op2 = alu->getShiftedData(op2, shift);
    } else{
        op2 = alu->getShiftedData(op2);
    }
    uint64_t result = op1 + op2;
    DEBUG_OUT<<"result = "<< hex << result << endl;
    reg->setReg(decodedInstruction->getRegDest(), result);
    if (alu->shouldUpdatePSR())
        reg->setFlags(NZCV, generateFlags(op1, op2, result));
}

void ArmCpu::subtractShifted(){
    ALUReg* alu = (ALUReg*) decodedInstruction;
    uint64_t op1 = reg->getReg(alu->getRegN());
    if (alu->getRegN()==PC)
        op1+=2*WORD_SIZE; //Rn is PC+12 for shift by reg
    uint32_t op2 = reg->getReg(alu->getRegM());
    if (alu->getRegM()==PC)
        op2+=2*WORD_SIZE; //Rm is PC+12 for shift by reg
    if(alu->shouldUseShiftByReg()){
        int shift = reg->getReg(alu->getRegShift()) & 0xFF;
        op2 = alu->getShiftedData(op2, shift);
    } else{
        op2 = alu->getShiftedData(op2);
    }
    uint64_t result = op1 - op2;
    DEBUG_OUT<<"result = "<< hex << result << endl;
    reg->setReg(decodedInstruction->getRegDest(), result);
    if (alu->shouldUpdatePSR())
        reg->setFlags(NZCV, generateFlags(op1, op2, result));
}

void ArmCpu::cmpShifted(){
    ALUReg* alu = (ALUReg*) decodedInstruction;
    uint64_t op1 = reg->getReg(alu->getRegN());
    uint32_t op2 = reg->getReg(alu->getRegM());
    if(alu->shouldUseShiftByReg()){
        int shift = reg->getReg(alu->getRegShift()) & 0xFF;
        op2 = alu->getShiftedData(op2, shift);
    } else{
        op2 = alu->getShiftedData(op2);
    }
    uint64_t result = op1 - op2;
    DEBUG_OUT<<"result = "<< hex << result << endl;
    reg->setFlags(NZCV, generateFlags(op1, -op2, result));
}

void ArmCpu::moveShifted(){
    ALUReg* alu = (ALUReg*) decodedInstruction;
    uint32_t data = reg->getReg(alu->getRegM());
    bool carry;
    if(alu->shouldUseShiftByReg()){
        int shift = reg->getReg(alu->getRegShift()) & 0xFF;
        carry = alu->getShiftedCarry(data, shift);
        data = alu->getShiftedData(data, shift);
    } else {
        carry = alu->getShiftedCarry(data);
        data = alu->getShiftedData(data);
    }
    DEBUG_OUT<<"data = " << data <<", carry = "<< carry << endl;
    reg->setReg(alu->getRegDest(), data);
    char mask=NZC;
    if (!alu->getShiftType() && !alu->getImmediate()) mask = NZ; //LSL#0 does not change carry flag
    if (alu->shouldUpdatePSR())
        reg->setFlags(mask, generateShiftFlags(carry, data));
}

void ArmCpu::orShifted(){
    ALUReg* alu = (ALUReg*) decodedInstruction;
    uint32_t op1 = reg->getReg(alu->getRegN());
    if (alu->getRegN()==PC)
        op1+=2*WORD_SIZE; //Rn is PC+12 for shift by reg
    uint32_t op2 = reg->getReg(alu->getRegM());
    if (alu->getRegM()==PC)
        op2+=2*WORD_SIZE; //Rm is PC+12 for shift by reg
    bool carry;
    if(alu->shouldUseShiftByReg()){
        int shift = reg->getReg(alu->getRegShift()) & 0xFF;
        carry = alu->getShiftedCarry(op2, shift);
        op2 = alu->getShiftedData(op2, shift);
    } else{
        carry = alu->getShiftedCarry(op2);
        op2 = alu->getShiftedData(op2);
    }
    uint32_t result = op1 | op2;
    DEBUG_OUT<<"result = " << result<<", carry = "<< carry << endl;
    reg->setReg(decodedInstruction->getRegDest(), result);
    char mask=NZC;
    if (!alu->getShiftType() && !alu->getImmediate()) mask = NZ; //LSL#0 does not change carry flag
    if (alu->shouldUpdatePSR())
        reg->setFlags(mask, generateShiftFlags(carry, result));
}

void ArmCpu::bitClearShifted(){
    ALUReg* alu = (ALUReg*) decodedInstruction;
    uint32_t op1 = reg->getReg(alu->getRegN());
    if (alu->getRegN()==PC)
        op1+=2*WORD_SIZE; //Rn is PC+12 for shift by reg
    uint32_t op2 = reg->getReg(alu->getRegM());
    if (alu->getRegM()==PC)
        op2+=2*WORD_SIZE; //Rm is PC+12 for shift by reg
    bool carry;
    if(alu->shouldUseShiftByReg()){
        int shift = reg->getReg(alu->getRegShift()) & 0xFF;
        carry = alu->getShiftedCarry(op2, shift);
        op2 = alu->getShiftedData(op2, shift);
    } else{
        carry = alu->getShiftedCarry(op2);
        op2 = alu->getShiftedData(op2);
    }
    uint32_t result = op1 & ~op2;
    DEBUG_OUT<<"result = " << result<<", carry = "<< carry << endl;
    reg->setReg(decodedInstruction->getRegDest(), result);
    char mask=NZC;
    if (!alu->getShiftType() && !alu->getImmediate()) mask = NZ; //LSL#0 does not change carry flag
    if (alu->shouldUpdatePSR())
        reg->setFlags(mask, generateShiftFlags(carry, result));
}
