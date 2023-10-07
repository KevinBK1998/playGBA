#include <iostream>
#include <string.h>
#include "Registers.cpp"
#include "ArmInstructions/Instruction.cpp"
#include "ArmInstructions/SingleDataTransfer.h"
#include "ArmInstructions/ALU.h"
#include "Memory.cpp"

using namespace std;

const int N = 0x80000000;
const int Z = 0x40000000;

int rotateleft(int data, int shift){
    return (data<<shift) | (data >> (32-shift));
}

class ArmCpu
{
private:
    Registers* reg;
    Memory mem;
    ArmInstruction* decodedInstruction = new ArmInstruction();
    int fetchedPC;
    int setFlags(int result){
        int flags = 0;
        if (((result >> 31) & 1) != 0)
            flags |= N;
        if (((int) result) == 0)
            flags |= Z;
        return flags;
    }
    void branch();
    void psrTransfer();
    void testXOR();
    void compare();
    void logicalOR();
    void move();
    void loadToReg();
    void storeFromReg();
    bool canExecute(Condition);
public:
    long time=0;
    ArmCpu(){
    }
    ArmCpu(Registers* registers){
        reg = registers;
    }
    ~ArmCpu(){
    }

    void decode(){
        int currentPC = reg->getPC();
        int opcode = mem.read32(currentPC);
        fetchedPC = currentPC + 4;
        if (((opcode>>26) & 0b11) == 0b00)
            decodedInstruction=ArmAluInstruction::decodeALU(opcode);
        else if (((opcode>>26) & 0b11) == 0b01)
            decodedInstruction=SingleDataTransfer::decodeSDT(opcode);
        else if (((opcode>>25) & 0b111) == 0b101){
            Condition cond = Condition((opcode >> 28) & 0xF);
            if ((opcode>>24) & 0b1)
                cout << "Incomplete BL{X} label = " << opcode << endl;
            int imm = opcode & 0xFFFFFF;
            decodedInstruction = new ArmInstruction(cond, B, imm);
        }
        else{
            cout << "Undefined Opcode: " << hex << opcode << endl;
            exit(FAILED_TO_DECODE);
        }
    }

    void execute(){
        if (decodedInstruction->getOpcode() == NOT_INITIALISED)
            return;
        cout << "Debug Execute: " << hex << decodedInstruction->toString() << endl;
        if (canExecute(decodedInstruction->condition)){
            switch (decodedInstruction->getOpcode())
            {
            case B:
                branch();
                break;
            case MRS:
            case MSR:
                psrTransfer();
                break;
            case TEQ:
                testXOR();
                break;
            case CMP:
                compare();
                break;
            case ORR:
                logicalOR();
                break;
            case MOV:
                move();
                break;
            case LDR:
                loadToReg();
                break;
            case STR:
                storeFromReg();
                break;
            default:
                cout << "Undefined: " << decodedInstruction->toString() << endl;
                exit(FAILED_TO_EXECUTE);
            }
        } else cout << "Skipping, condition failed" << endl;
        time++;
    }

    void step(){
        execute();
        decode();
        reg->step();
    }
};

bool ArmCpu::canExecute(Condition cond){
    int status = reg->getCPSR();
    switch (cond.value)
    {
    case EQ:
        return (status & Z) != 0;
    case ALWAYS:
        return true;
    default:
        cout << "Undefined: " << cond.toString() << endl;
        exit(FAILED_TO_EXECUTE);
    }
    return false;
        // if (cond == MI) return reg->getCPSR() < 0;
        // else if (cond == PL) return currentStatusReg >= 0;
        // else if (cond == NE) return (currentStatusReg & 0x40_00_00_00) == 0;
        // else if (cond == CS) return (currentStatusReg & 0x20_00_00_00) != 0;
        // else if (cond == CC) return (currentStatusReg & 0x20_00_00_00) == 0;
        // else if (cond == VS) return (currentStatusReg & 0x10_00_00_00) != 0;
        // else if (cond == VC) return (currentStatusReg & 0x10_00_00_00) == 0;
        // else if (cond == HI) return canExecute(CS) && canExecute(NE);
        // else if (cond == LS) return canExecute(CC) || canExecute(EQ);
        // else if (cond == GE) return canExecute(MI) == canExecute(VS);
        // else if (cond == LT) return canExecute(MI) != canExecute(VS);
        // else if (cond == GT) return canExecute(NE) && canExecute(GE);
        // else if (cond == LE) return canExecute(EQ) || canExecute(LT);
        // else throw new IndexOutOfBoundsException(cond.name());
}

void ArmCpu::branch(){
    reg->branch(decodedInstruction->getImmediate());
}

void ArmCpu::psrTransfer(){
    if (decodedInstruction->getOpcode() == MRS){
        int data = decodedInstruction->getImmediate() ? reg->getSPSR(): reg->getCPSR();
        cout<<"result = "<< hex << data << endl;
        reg->setReg(decodedInstruction->getRegDest(), data);
    }
    else{
        ArmAluInstruction* psrTfr = (ArmAluInstruction*) decodedInstruction;
        int result = psrTfr->getMask() & reg->getReg(psrTfr->getRegN());
        cout<<"mask = "<<psrTfr->getMask()<<"\tresult = "<<result<<endl;
        if(decodedInstruction->getImmediate())
            reg->setSPSR(result);
        else
            reg->setCPSR(result);
    }
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

void ArmCpu::move(){
    int immediate = decodedInstruction->getImmediate();
    reg->setReg(decodedInstruction->getRegDest(), immediate);
    cout<<"result = "<< hex << immediate << endl;
    // if (alu.canChangePsr())
    //     reg->setCPSR(setFlags(immediate));
}

void ArmCpu::loadToReg(){
    SingleDataTransfer* sdt = (SingleDataTransfer*) decodedInstruction;
    int regNValue = reg->getReg(decodedInstruction->getRegN());
    int address = regNValue + decodedInstruction->getImmediate();
    int data;
    if (sdt->byteTransfer)
        data = mem.read8(address);
    else
        data = mem.read32(address);
    cout<<"data = "<< data << endl;
    reg->setReg(decodedInstruction->getRegDest(), data);
}

void ArmCpu::storeFromReg(){
    SingleDataTransfer* sdt = (SingleDataTransfer*) decodedInstruction;
    int regNValue = reg->getReg(decodedInstruction->getRegN());
    int address = regNValue + decodedInstruction->getImmediate();
    int data = reg->getReg(decodedInstruction->getRegDest());
    if (sdt->byteTransfer){
        cout<<"data = "<< (data & 0xFF) << endl;
        mem.write8(address, data);
    }
    else{
        mem.write32(address, data);
        cout<<"data = "<< data << endl;
    }
}
