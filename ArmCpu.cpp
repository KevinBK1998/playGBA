#include <iostream>
#include <string.h>
#include "ArmCpu.h"
#include "ArmInstructions/Instruction.cpp"
#include "ArmInstructions/Branch.h"
#include "ArmInstructions/SingleDataTransfer.cpp"
#include "ArmInstructions/MultipleDataTransfer.cpp"
#include "ArmInstructions/ALU.cpp"
#include "ArmInstructions/ALUReg.cpp"
#include "ArmInstructions/PSRTransfer.cpp"
#include "ArmInstructions/ARMExtendedSDT.cpp"

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
    else if (((opcode>>25) & 0b111) == 0b000){
        if (((opcode>>4) & 1) && ((opcode>>7) & 1))
            decodedInstruction=ARMExtendedSDT::decode(opcode);
        else
            decodedInstruction=ALUReg::decode(opcode);
    }
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
        case RSB:
            reverseSubtract();
            break;
        case LDRH:
            loadHalfReg();
            break;
        case EOR:
            exclusiveOR();
            break;
        case ADC:
            addWithCarryShifted();
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

bool ArmCpu::canExecute(ArmInstruction* instruction){
    Condition cond = instruction->getPreCheck();
    return reg->canExecute(cond.value);
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

void ArmCpu::logicalAND(){
    if(decodedInstruction->shouldUseImmediate())
        andImmediate();
    else
        andShifted();
}

void ArmCpu::add(){
    if(decodedInstruction->shouldUseImmediate())
        addImmediate();
    else
        addShifted();
}

void ArmCpu::subtract(){
    if(decodedInstruction->shouldUseImmediate())
        subtractImmediate();
    else
        subtractShifted();
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

void ArmCpu::logicalOR(){
    if(decodedInstruction->shouldUseImmediate())
        orImmediate();
    else
        orShifted();
}

void ArmCpu::bitClear(){
    if(decodedInstruction->shouldUseImmediate())
        bitClearImmediate();
    else
        bitClearShifted();
}

void ArmCpu::testXOR(){
    if(decodedInstruction->shouldUseImmediate())
        testXORImmediate();
    else
        testXORShifted();
}
