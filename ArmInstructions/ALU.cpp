#include <sstream>
#include "../ArmCpu.h"

int rotateRight(int data, int shift){
    return (data>>shift) | (data << (32-shift));
}

class ALU : public ArmInstruction
{
private:
public:
    ALU(Opcode opcode, Condition cond, char destReg, char op1, int imm): ArmInstruction(cond, opcode, op1, destReg, imm){}
    ALU(Opcode opcode, Condition cond, int imm, char op1): ArmInstruction(cond, opcode, op1, imm){}
    ALU(Opcode opcode, Condition cond, char destReg, int imm): ArmInstruction(cond, opcode, imm, destReg){}

    static ALU* decode(int opcode){
        Condition cond = Condition((opcode >> 28) & 0xF);
        char rN = (opcode >> 16) & 0xF;
        char rDest = (opcode >> 12) & 0xF;
        int shift = (opcode >> 8) & 0xF;
        int imm = opcode & 0xFF;
        imm = rotateRight(imm, shift*2);
        switch ((opcode >> 21) & 0xF)
        {
        case 0:
            return new ALU(AND, cond, rDest, rN, imm);
        case 2:
            return new ALU(SUB, cond, rDest, rN, imm);
        case 4:
            return new ALU(ADD, cond, rDest, rN, imm);
        case 9:
            return new ALU(TEQ, cond, imm, rN);
        case 0xA:
            return new ALU(CMP, cond, imm, rN);
        case 0xC:
            return new ALU(ORR, cond, rDest, rN, imm);
        case 0xD:
            return new ALU(MOV, cond, rDest, imm);
        case 0xE:
            return new ALU(BIC, cond, rDest, rN, imm);
        }
        cout << "ALU = " << opcode << ", Funcode = " << ((opcode >> 21) & 0xF) << endl;
        exit(FAILED_TO_DECODE);
    }

    bool shouldUseImmediate(){
        return true;
    }

    string toString(){
        stringstream stream;
        switch (getOpcode())
        {
        case AND:
            stream<< "AND"<<getCondition()<<" R" << getRegDest() << ", R" << getRegN();
            break;
        case SUB:
            stream<< "SUB"<<getCondition()<<" R" << getRegDest() << ", R" << getRegN();
            break;
        case ADD:
            stream<< "ADD"<<getCondition()<<" R" << getRegDest() << ", R" << getRegN();
            break;
        case TEQ:
            stream<< "TEQ"<<getCondition()<<" R" << getRegN();
            break;
        case CMP:
            stream<< "CMP"<<getCondition()<<" R" << getRegN();
            break;
        case ORR:
            stream<< "ORR"<<getCondition()<<" R" << getRegDest() << ", R" << getRegN();
            break;
        case MOV:
            stream<< "MOV"<<getCondition()<<" R" << getRegDest();
            break;
        case BIC:
            stream<< "BIC"<<getCondition()<<" R" << getRegDest() << ", R" << getRegN();
            break;
        default:
            cout << "ALU = " << hex << getOpcode() << endl;
            exit(FAILED_DECODED_TO_STRING);
        }
        stream<<showbase<<hex<<", "<<getImmediate();
        return stream.str();
    }
};

void ArmCpu::logicalAND(){
    int result = reg->getReg(decodedInstruction->getRegN()) & decodedInstruction->getImmediate();
    reg->setReg(decodedInstruction->getRegDest(), result);
    cout<<"result = "<< hex << result << endl;
    // if (alu.canChangePsr())
    //     reg->setCPSR(setFlags(result));
}

void ArmCpu::subtract(){
    int before = reg->getReg(decodedInstruction->getRegN());
    int immediate = decodedInstruction->getImmediate();
    int result = before - immediate;
    cout<<"result = "<< hex << result << endl;
    reg->setReg(decodedInstruction->getRegDest(), result);
    // if (alu.canChangePsr())
    //     reg->setCPSR(setFlags(result));
}

void ArmCpu::add(){
    int before = reg->getReg(decodedInstruction->getRegN());
    int immediate = decodedInstruction->getImmediate();
    int result = before + immediate;
    cout<<"result = "<< hex << result << endl;
    reg->setReg(decodedInstruction->getRegDest(), result);
    // if (alu.canChangePsr())
    //     reg->setCPSR(setFlags(result));
}

void ArmCpu::testXOR(){
    int before = reg->getReg(decodedInstruction->getRegN());
    int immediate = decodedInstruction->getImmediate();
    int result = before ^ immediate;
    cout<<"result = "<< hex << result << endl;
    reg->setCPSR(setFlags(result));
}

void ArmCpu::compare(){
    int before = reg->getReg(decodedInstruction->getRegN());
    int immediate = decodedInstruction->getImmediate();
    int result = before - immediate;
    cout<<"result = "<< hex << result << endl;
    reg->setCPSR(setFlags(result));
}

void ArmCpu::logicalOR(){
    int result = reg->getReg(decodedInstruction->getRegN()) | decodedInstruction->getImmediate();
    reg->setReg(decodedInstruction->getRegDest(), result);
    cout<<"result = "<< hex << result << endl;
    // if (alu.canChangePsr())
    //     reg->setCPSR(setFlags(result));
}

void ArmCpu::moveImmediate(){
    int immediate = decodedInstruction->getImmediate();
    reg->setReg(decodedInstruction->getRegDest(), immediate);
    cout<<"result = "<< hex << immediate << endl;
    // if (alu.canChangePsr())
    //     reg->setCPSR(setFlags(immediate));
}

void ArmCpu::bitClear(){
    int result = reg->getReg(decodedInstruction->getRegN()) & ~decodedInstruction->getImmediate();
    reg->setReg(decodedInstruction->getRegDest(), result);
    cout<<"result = "<< hex << result << endl;
    // if (alu.canChangePsr())
    //     reg->setCPSR(setFlags(result));
}
