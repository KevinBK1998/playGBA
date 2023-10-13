#include <iostream>
#include <string.h>
#include "ArmCpu.h"
#include "ArmInstructions/Instruction.cpp"
#include "ArmInstructions/Branch.h"
#include "ArmInstructions/SingleDataTransfer.h"
#include "ArmInstructions/MultipleDataTransfer.cpp"
#include "ArmInstructions/ALU.cpp"
#include "ArmInstructions/ALUReg.cpp"
#include "ArmInstructions/PSRTransfer.cpp"

using namespace std;

int rotateleft(int data, int shift){
    return (data<<shift) | (data >> (32-shift));
}

ArmCpu::ArmCpu(Registers* registers, Memory* memory){
    reg = registers;
    mem = memory;
}

void ArmCpu::decode(){
    int currentPC = reg->getPC();
    int opcode = mem->read32(currentPC);
    DEBUG_OUT <<showbase<< "Debug Opcode: " << opcode << endl;
    if (((opcode>>8) & 0xFFFFF) == 0x12FFF)
        decodedInstruction=Branch::decode(opcode, true);
    else if (((opcode>>20) & 0b11011001) == 0b10000)
        decodedInstruction=PSRTransfer::decode(opcode);
    else if (((opcode>>25) & 0b111) == 0b000)
        decodedInstruction=ALUReg::decode(opcode);
    else if (((opcode>>25) & 0b111) == 0b001)
        decodedInstruction=ALU::decode(opcode);
    else if (((opcode>>25) & 0b111) == 0b100)
        decodedInstruction=MultipleDataTransfer::decode(opcode);
    else if (((opcode>>25) & 0b111) == 0b101)
        decodedInstruction=Branch::decode(opcode);
    else if (((opcode>>26) & 0b11) == 0b01)
        decodedInstruction=SingleDataTransfer::decodeSDT(opcode);
    else{
        cout << "Undecoded Opcode: " << hex << opcode << endl;
        exit(FAILED_TO_DECODE);
    }
}

void ArmCpu::execute(){
    if (decodedInstruction->getOpcode() == NOT_INITIALISED){
        DEBUG_OUT << "No cached Instruction, skipping" << endl;
        return;
    }
    DEBUG_OUT << "Debug Execute: " << hex << decodedInstruction->toString() << endl;
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
        case TST:
            test();
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
        case BIC:
            bitClear();
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
    } else DEBUG_OUT << "Skipping, condition failed" << endl;
    time++;
}

void ArmCpu::step(){
    decode();
    execute();
    if (reg->isThumbMode()){
        decodedInstruction = new ArmInstruction();
        return;
    }
    reg->step();
}

bool ArmCpu::canExecute(int cond){
    int status = reg->getCPSR();
    switch (cond)
    {
    case EQ:
        return (status & Z) != 0;
    case NE:
        return (status & Z) == 0;
    case CC:
        return (status & C) == 0;
    case MI:
        return (status & N) != 0;
    case VS:
        return (status & V) != 0;
    case LT:
        return canExecute(MI) != canExecute(VS);
    case ALWAYS:
        return true;
    default:
        cout << "Undefined: " << Condition(cond).toString() << endl;
        exit(FAILED_TO_EXECUTE);
    }
    // if (cond == PL) return currentStatusReg >= 0;
    // else if (cond == CS) return (currentStatusReg & 0x20_00_00_00) != 0;
    // else if (cond == CC) return (currentStatusReg & 0x20_00_00_00) == 0;
    // else if (cond == VC) return (currentStatusReg & 0x10_00_00_00) == 0;
    // else if (cond == HI) return canExecute(CS) && canExecute(NE);
    // else if (cond == LS) return canExecute(CC) || canExecute(EQ);
    // else if (cond == GE) return canExecute(MI) == canExecute(VS);
    // else if (cond == LT) return canExecute(MI) != canExecute(VS);
    // else if (cond == GT) return canExecute(NE) && canExecute(GE);
    // else if (cond == LE) return canExecute(EQ) || canExecute(LT);
    // else throw new IndexOutOfBoundsException(cond.name());
    return false;
}

bool ArmCpu::canExecute(ArmInstruction* instruction){
    Condition cond = instruction->getPreCheck();
    return canExecute(cond.value);
}

void ArmCpu::branch(){
    Branch* b = (Branch*) decodedInstruction;
    if(b->shouldSavePC())
        reg->setReg(LR, reg->getPC() + WORD_SIZE);
    int data = b->getImmediate();
    reg->branch(data);
}

void ArmCpu::branchExchange(){
    Branch* b = (Branch*) decodedInstruction;
    if(b->shouldSavePC())
        reg->setReg(LR, reg->getPC() + WORD_SIZE);
    int data = reg->getReg(b->getRegN());
    DEBUG_OUT<<"jumpAddress = "<<data<<endl;
    reg->exchange(data);
}

void ArmCpu::loadReg(){
    SingleDataTransfer* sdt = (SingleDataTransfer*) decodedInstruction;
    int regNValue = reg->getReg(sdt->getRegN());
    if (sdt->getRegN()==PC)
        regNValue+=WORD_SIZE; //Base Register is PC+8
    int address = regNValue + sdt->getImmediate();
    int data;
    if (sdt->isByteTransfer())
        data = mem->read8(address);
    else
        data = mem->read32(address);
    DEBUG_OUT<<"address = "<<address<<", data = "<< data << endl;
    reg->setReg(sdt->getRegDest(), data);
}

void ArmCpu::storeReg(){
    SingleDataTransfer* sdt = (SingleDataTransfer*) decodedInstruction;
    int regNValue = reg->getReg(sdt->getRegN());
    if (sdt->getRegN()==PC)
        regNValue+=WORD_SIZE; //Base Register is PC+8
    int data = reg->getReg(sdt->getRegDest());
    if (sdt->getRegDest()==PC)
        data+=2*WORD_SIZE; //Dest Register is PC+12
    int address = regNValue + sdt->getImmediate();
    if (sdt->isByteTransfer()){
        DEBUG_OUT<<"address = "<<address<<", data = "<< (data & 0xFF) << endl;
        mem->write8(address, data);
    }
    else{
        DEBUG_OUT<<"address = "<<address<<", data = "<< data << endl;
        mem->write32(address, data);
    }
}

void ArmCpu::add(){
    if(decodedInstruction->shouldUseImmediate())
        addImmediate();
    else
        addShifted();
}

void ArmCpu::move(){
    if(decodedInstruction->shouldUseImmediate())
        moveImmediate();
    else
        moveShifted();
}

void ArmCpu::compare(){
    if(decodedInstruction->shouldUseImmediate())
        cmpImmediate();
    else
        cmpShifted();
}
