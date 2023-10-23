#include <sstream>
#include "../ArmCpu.h"

void exitIfOpcodeP(char r){
    if (r) {
        cout << "P opcodes = " << unsigned(r) << endl;
        exit(PENDING_CODE);
    }
}

class ALU : public ArmInstruction
{
private:
    bool updatePSR;
    string getPSRToString(){
        return updatePSR?"S":"";
    }
public:
    ALU(Opcode opcode, Condition cond, bool psr, char destReg, char op1, int imm): ArmInstruction(cond, opcode, op1, destReg, imm){
        updatePSR = psr;
    }
    ALU(Opcode opcode, Condition cond, int imm, char op1): ArmInstruction(cond, opcode, op1, imm){
        updatePSR = true;
    }
    ALU(Opcode opcode, Condition cond, bool psr, char destReg, int imm): ArmInstruction(cond, opcode, imm, destReg){
        updatePSR = psr;
    }

    static ALU* decode(int opcode){
        Condition cond = Condition((opcode >> 28) & 0xF);
        char op = (opcode >> 21) & 0xF;
        bool psr = (opcode>>20) & 1;
        char rN = (opcode >> 16) & 0xF;
        char rDest = (opcode >> 12) & 0xF;
        int shift = (opcode >> 8) & 0xF;
        int imm = opcode & 0xFF;
        imm = ROR(imm, shift*2);
        switch (op)
        {
        case 0:
            return new ALU(AND, cond, psr, rDest, rN, imm);
        case 2:
            return new ALU(SUB, cond, psr, rDest, rN, imm);
        case 3:
            return new ALU(RSB, cond, psr, rDest, rN, imm);
        case 4:
            return new ALU(ADD, cond, psr, rDest, rN, imm);
        case 8:
            exitIfOpcodeP(rDest);
            return new ALU(TST, cond, imm, rN);
        case 9:
            exitIfOpcodeP(rDest);
            return new ALU(TEQ, cond, imm, rN);
        case 0xA:
            exitIfOpcodeP(rDest);
            return new ALU(CMP, cond, imm, rN);
        case 0xC:
            return new ALU(ORR, cond, psr, rDest, rN, imm);
        case 0xD:
            return new ALU(MOV, cond, psr, rDest, imm);
        case 0xE:
            return new ALU(BIC, cond, psr, rDest, rN, imm);
        }
        cout << "ALU = " << opcode << ", Funcode = " << ((opcode >> 21) & 0xF) << endl;
        exit(FAILED_TO_DECODE);
    }

    bool shouldUseImmediate(){
        return true;
    }

    bool shouldUpdatePSR(){
        return updatePSR;
    }

    string toString(){
        stringstream stream;
        switch (getOpcode())
        {
        case AND:
            stream<< "AND"<<getPSRToString()<<getCondition()<<" R" << getRegDest() << ", R" << getRegN();
            break;
        case SUB:
            stream<< "SUB"<<getPSRToString()<<getCondition()<<" R" << getRegDest() << ", R" << getRegN();
            break;
        case RSB:
            stream<< "RSB"<<getPSRToString()<<getCondition()<<" R" << getRegDest() << ", R" << getRegN();
            break;
        case ADD:
            stream<< "ADD"<<getPSRToString()<<getCondition()<<" R" << getRegDest() << ", R" << getRegN();
            break;
        case TST:
            stream<< "TST"<<getCondition()<<" R" << getRegN();
            break;
        case TEQ:
            stream<< "TEQ"<<getCondition()<<" R" << getRegN();
            break;
        case CMP:
            stream<< "CMP"<<getCondition()<<" R" << getRegN();
            break;
        case ORR:
            stream<< "ORR"<<getPSRToString()<<getCondition()<<" R" << getRegDest() << ", R" << getRegN();
            break;
        case MOV:
            stream<< "MOV"<<getPSRToString()<<getCondition()<<" R" << getRegDest();
            break;
        case BIC:
            stream<< "BIC"<<getPSRToString()<<getCondition()<<" R" << getRegDest() << ", R" << getRegN();
            break;
        default:
            cout << "ALU = " << hex << getOpcode() << endl;
            exit(FAILED_DECODED_TO_STRING);
        }
        stream<<showbase<<hex<<", "<<getImmediate();
        return stream.str();
    }
};

void ArmCpu::andImmediate(){
    ALU* alu = (ALU*) decodedInstruction;
    int result = reg->getReg(alu->getRegN()) & alu->getImmediate();
    reg->setReg(alu->getRegDest(), result);
    DEBUG_OUT<<"result = "<< hex << result << endl;
    if (alu->shouldUpdatePSR())
        reg->setFlags(NZ, generateFlags(result));
}

void ArmCpu::subtractImmediate(){
    ALU* alu = (ALU*) decodedInstruction;
    uint64_t before = reg->getReg(alu->getRegN());
    uint32_t immediate = alu->getImmediate();
    uint64_t result = before - immediate;
    DEBUG_OUT<<"result = "<< hex << result << endl;
    reg->setReg(alu->getRegDest(), result);
    if (alu->shouldUpdatePSR())
        reg->setFlags(NZCV, generateFlags(result));
}

void ArmCpu::reverseSubtract(){
    ALU* alu = (ALU*) decodedInstruction;
    uint64_t op1 = alu->getImmediate();
    uint32_t op2 = reg->getReg(alu->getRegN());
    uint64_t result = op1 - op2;
    DEBUG_OUT<<"result = "<< hex << result << endl;
    reg->setReg(alu->getRegDest(), result);
    if (alu->shouldUpdatePSR())
        reg->setFlags(NZCV, generateFlags(result));
}

void ArmCpu::addImmediate(){
    ALU* alu = (ALU*) decodedInstruction;
    uint64_t before = reg->getReg(alu->getRegN());
    if (alu->getRegN()==PC)
        before+=WORD_SIZE; //Base Register is PC+8
    uint32_t immediate = alu->getImmediate();
    uint64_t result = before + immediate;
    DEBUG_OUT<<"result = "<< hex << result << endl;
    reg->setReg(alu->getRegDest(), result);
    if (alu->shouldUpdatePSR())
        reg->setFlags(NZCV, generateFlags(before, immediate, result));
}

void ArmCpu::test(){
    int before = reg->getReg(decodedInstruction->getRegN());
    int immediate = decodedInstruction->getImmediate();
    int result = before & immediate;
    DEBUG_OUT<<"result = "<< hex << result << endl;
    reg->setFlags(NZ, generateFlags(result));
}

void ArmCpu::testXORImmediate(){
    int before = reg->getReg(decodedInstruction->getRegN());
    int immediate = decodedInstruction->getImmediate();
    int result = before ^ immediate;
    DEBUG_OUT<<"result = "<< hex << result << endl;
    reg->setFlags(NZ, generateFlags(result));
}

void ArmCpu::cmpImmediate(){
    uint64_t before = reg->getReg(decodedInstruction->getRegN());
    uint32_t immediate = decodedInstruction->getImmediate();
    uint64_t result = before - immediate;
    DEBUG_OUT<<"result = "<< hex << result << endl;
    reg->setFlags(NZCV, generateFlags(before, -immediate, result));
}

void ArmCpu::orImmediate(){
    ALU* alu = (ALU*) decodedInstruction;
    int result = reg->getReg(alu->getRegN()) | alu->getImmediate();
    reg->setReg(alu->getRegDest(), result);
    DEBUG_OUT<<"result = "<< hex << result << endl;
    if (alu->shouldUpdatePSR())
        reg->setFlags(NZ, generateFlags(result));
}

void ArmCpu::moveImmediate(){
    ALU* alu = (ALU*) decodedInstruction;
    int immediate = alu->getImmediate();
    reg->setReg(alu->getRegDest(), immediate);
    DEBUG_OUT<<"result = "<< hex << immediate << endl;
    if (alu->shouldUpdatePSR())
        reg->setFlags(NZ, generateFlags(immediate));
}

void ArmCpu::bitClearImmediate(){
    ALU* alu = (ALU*) decodedInstruction;
    int result = reg->getReg(alu->getRegN()) & ~alu->getImmediate();
    reg->setReg(alu->getRegDest(), result);
    DEBUG_OUT<<"result = "<< hex << result << endl;
    if (alu->shouldUpdatePSR())
        reg->setFlags(NZ, generateFlags(result));
}
