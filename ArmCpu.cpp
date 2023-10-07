#include <iostream>
#include <string.h>
#include "Registers.h"
#include "Memory.h"
#include "ArmInstructions/Instruction.cpp"
#include "ArmInstructions/Branch.h"
#include "ArmInstructions/SingleDataTransfer.h"
#include "ArmInstructions/MultipleDataTransfer.h"
#include "ArmInstructions/ALU.h"

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
    Memory* mem;
    ArmInstruction* decodedInstruction = new ArmInstruction();
    int setFlags(int result){
        int flags = 0;
        if (((result >> 31) & 1) != 0)
            flags |= N;
        if (((int) result) == 0)
            flags |= Z;
        return flags;
    }
    void branch();
    void branchExchange();
    void psrTransfer();
    void logicalAND();
    void subtract();
    void add();
    void testXOR();
    void compare();
    void logicalOR();
    void move();
    void loadReg();
    void storeReg();
    void loadMultipleReg();
    void storeMultipleReg();
    bool canExecute(ArmInstruction*);
public:
    long time=0;
    ArmCpu(){
    }
    ArmCpu(Registers* registers, Memory* memory){
        reg = registers;
        mem = memory;
    }
    ~ArmCpu(){
    }

    void decode(){
        int currentPC = reg->getPC();
        int opcode = mem->read32(currentPC);
        if (((opcode>>8) & 0xFFFFF) == 0x12FFF)
            decodedInstruction=Branch::decode(opcode, true);
        else if (((opcode>>25) & 0b111) == 0b100)
            decodedInstruction=MultipleDataTransfer::decodeMDT(opcode);
        else if (((opcode>>25) & 0b111) == 0b101)
            decodedInstruction=Branch::decode(opcode);
        else if (((opcode>>26) & 0b11) == 0b00)
            decodedInstruction=ALU::decodeALU(opcode);
        else if (((opcode>>26) & 0b11) == 0b01)
            decodedInstruction=SingleDataTransfer::decodeSDT(opcode);
        else{
            cout << "Undecoded Opcode: " << hex << opcode << endl;
            exit(FAILED_TO_DECODE);
        }
    }

    void execute(){
        if (decodedInstruction->getOpcode() == NOT_INITIALISED)
            return;
        cout << "Debug Execute: " << hex << decodedInstruction->toString() << endl;
        if (canExecute(decodedInstruction)){
            switch (decodedInstruction->getOpcode())
            {
            case B:
                branch();
                break;
            case BX:
                branchExchange();
                break;
            case AND:
                logicalAND();
                break;
            case SUB:
                subtract();
                break;
            case ADD:
                add();
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
                loadReg();
                break;
            case STR:
                storeReg();
                break;
            case LDM:
                loadMultipleReg();
                break;
            case STM:
                storeMultipleReg();
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
        if (!reg->isThumbMode()){
            decode();
            reg->step();
        }
    }
};

bool ArmCpu::canExecute(ArmInstruction* instruction){
    Condition cond = instruction->getPreCheck();
    int status = reg->getCPSR();
    switch (cond.value)
    {
    case EQ:
        return (status & Z) != 0;
    case NE:
        return (status & Z) == 0;
    case ALWAYS:
        return true;
    default:
        cout << "Undefined: " << cond.toString() << endl;
        exit(FAILED_TO_EXECUTE);
    }
    return false;
        // if (cond == MI) return reg->getCPSR() < 0;
        // else if (cond == PL) return currentStatusReg >= 0;
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
    Branch* b = (Branch*) decodedInstruction;
    if(b->shouldSavePC())
        reg->setReg(LR, reg->getReg(PC) + WORD_SIZE);
    int data = b->getImmediate();
    reg->branch(data);
}

void ArmCpu::branchExchange(){
    Branch* b = (Branch*) decodedInstruction;
    if(b->shouldSavePC())
        reg->setReg(LR, reg->getReg(PC) + WORD_SIZE);
    int data = reg->getReg(b->getRegN());
    reg->exchange(data);
}

void ArmCpu::psrTransfer(){
    if (decodedInstruction->getOpcode() == MRS){
        int data = decodedInstruction->getImmediate() ? reg->getSPSR(): reg->getCPSR();
        cout<<"result = "<< hex << data << endl;
        reg->setReg(decodedInstruction->getRegDest(), data);
    }
    else{
        ALU* psrTfr = (ALU*) decodedInstruction;
        int result = psrTfr->getMask() & reg->getReg(psrTfr->getRegN());
        cout<<"mask = "<<psrTfr->getMask()<<"\tresult = "<<result<<endl;
        if(decodedInstruction->getImmediate())
            reg->setSPSR(result);
        else
            reg->setCPSR(result);
    }
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

void ArmCpu::logicalAND(){
    int result = reg->getReg(decodedInstruction->getRegN()) & decodedInstruction->getImmediate();
    reg->setReg(decodedInstruction->getRegDest(), result);
    cout<<"result = "<< hex << result << endl;
    // if (alu.canChangePsr())
    //     reg->setCPSR(setFlags(result));
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

void ArmCpu::loadReg(){
    SingleDataTransfer* sdt = (SingleDataTransfer*) decodedInstruction;
    int regNValue = reg->getReg(sdt->getRegN());
    int address = regNValue + sdt->getImmediate();
    int data;
    if (sdt->isByteTransfer())
        data = mem->read8(address);
    else
        data = mem->read32(address);
    cout<<"data = "<< data << endl;
    reg->setReg(sdt->getRegDest(), data);
}

void ArmCpu::storeReg(){
    SingleDataTransfer* sdt = (SingleDataTransfer*) decodedInstruction;
    int regNValue = reg->getReg(sdt->getRegN());
    int address = regNValue + sdt->getImmediate();
    int data = reg->getReg(sdt->getRegDest());
    if (sdt->isByteTransfer()){
        cout<<"data = "<< (data & 0xFF) << endl;
        mem->write8(address, data);
    }
    else{
        mem->write32(address, data);
        cout<<"data = "<< data << endl;
    }
}

void ArmCpu::loadMultipleReg(){
    MultipleDataTransfer* mdt = (MultipleDataTransfer*) decodedInstruction;
    int address = reg->getReg(mdt->getRegN());
    int data;
    int list = mdt->getRegList();
    int offset = mdt->shouldAddOffset()? WORD_SIZE: -WORD_SIZE;
    for (int i = 0; i < 16; i++){
        if (list & 1){
            if (mdt->addBeforeTransfer())
                address+=offset;
            data = mem->read32(address);
            cout<<"address = "<<address<<", R"<<dec<<i<<hex<<" = "<< data << endl;
            reg->setReg(i, data);
            if (!mdt->addBeforeTransfer())
                address+=offset;
        }
        list>>=1;
    }
    if (mdt->shouldWriteBack())
        reg->setReg(mdt->getRegN(),address);
}

void ArmCpu::storeMultipleReg(){
    MultipleDataTransfer* mdt = (MultipleDataTransfer*) decodedInstruction;
    int address = reg->getReg(mdt->getRegN());
    int data;
    int list = mdt->getRegList();
    int offset = mdt->shouldAddOffset()? WORD_SIZE: -WORD_SIZE;
    for (int i = 0; i < 16; i++){
        if (list & 1){
            if (mdt->addBeforeTransfer())
                address+=offset;
            data = reg->getReg(i);
            cout<<"address = "<<address<<", R"<<dec<<i<<hex<<" = "<< data << endl;
            mem->write32(address, data);
            if (!mdt->addBeforeTransfer())
                address+=offset;
        }
        list>>=1;
    }
    if (mdt->shouldWriteBack())
        reg->setReg(mdt->getRegN(),address);
}
