#include "ThumbCpu.h"
#include "ThumbInstructions/Instruction.cpp"
#include "ThumbInstructions/ThumbALU.cpp"
#include "ThumbInstructions/AddSP.h"
#include "ThumbInstructions/ALUImmediate.cpp"
#include "ThumbInstructions/ShiftedALU.cpp"
#include "ThumbInstructions/SingleDataTransfer.h"
#include "ThumbInstructions/LoadPCRelative.h"
#include "ThumbInstructions/SDTRelativeSP.h"
#include "ThumbInstructions/CondBranch.h"
#include "ThumbInstructions/Branch.cpp"
#include "ThumbInstructions/LongBranch.h"
#include "ThumbInstructions/SDTImmediate.h"
#include "ThumbInstructions/Add.cpp"
#include "ThumbInstructions/HiRegOperation.cpp"
#include "ThumbInstructions/SDTRegOffset.cpp"
#include "ThumbInstructions/SPThumbMDT.cpp"
#include "ThumbInstructions/ThumbMDT.cpp"
#include "ThumbInstructions/GetRelativeAddress.cpp"

ThumbCpu::ThumbCpu(Registers* registers, Memory* memory){
    reg = registers;
    mem = memory;
}

void ThumbCpu::decode(){
    int currentPC = reg->getPC();
    int opcode = mem->read16(currentPC);
    DEBUG_OUT <<showbase<< "Debug Opcode: " << opcode << endl;
    if (((opcode>>8) & 0xFF) == 0xB0)
        decodedInstruction = AddSP::decode(opcode);
    else if (((opcode>>10) & 0x3F)== 0b010000)
        decodedInstruction = ThumbALU::decode(opcode);
    else if (((opcode>>10) & 0x3F)== 0b10001)
        decodedInstruction = HiRegOperation::decode(opcode);
    else if (((opcode>>11) & 0x1F)== 0b11)
        decodedInstruction = AddRegImmediate::decode(opcode);
    else if (((opcode>>11) & 0x1F)== 0b1001)
        decodedInstruction = LoadPCRelative::decode(opcode);
    else if (((opcode>>11) & 0x1F)== 0b11100)
        decodedInstruction = ThumbBranch::decode(opcode);
    else if (((opcode>>12) & 0b1111)== 0b101)
        decodedInstruction = ThumbSDT::decode(opcode);
    else if (((opcode>>12) & 0b1111)== 0b1000)
        decodedInstruction = SDTHalfImmediate::decode(opcode);
    else if (((opcode>>12) & 0b1111)== 0b1001)
        decodedInstruction = SDTRelativeSP::decode(opcode);
    else if (((opcode>>12) & 0b1111)== 0b1010)
        decodedInstruction = GetRelativeAddress::decode(opcode);
    else if (((opcode>>12) & 0b1111)== 0b1011)
        decodedInstruction = SPThumbMDT::decode(opcode);
    else if (((opcode>>12) & 0b1111)== 0b1100)
        decodedInstruction = ThumbMDT::decode(opcode);
    else if (((opcode>>12) & 0b1111)== 0b1101)
        decodedInstruction = CondBranch::decode(opcode);
    else if (((opcode>>12) & 0b1111)== 0b1111)
        decodedInstruction = ThumbLongBranch::decode(opcode);
    else if (((opcode>>13) & 0b111)== 0)
        decodedInstruction = ShiftMove::decode(opcode);
    else if (((opcode>>13) & 0b111)== 1)
        decodedInstruction = ALUThumbIMM::decode(opcode);
    else if (((opcode>>13) & 0b111)== 0b11)
        decodedInstruction = SDTRegOffset::decode(opcode);
    else{
        cout << "Undecoded Opcode: " << opcode << endl;
        exit(FAILED_TO_DECODE);
    }
}
void ThumbCpu::execute(){
    if (decodedInstruction->getOpcode() == NOT_INITIALISED){
        DEBUG_OUT << "No cached Instruction, skipping" << endl;
        return;
    }
    DEBUG_OUT <<showbase<< "Debug Execute: " << decodedInstruction->toString() << endl;
    switch (decodedInstruction->getOpcode())
    {
    case MOV:
        move();
        break;
    case SUB:
        if(decodedInstruction->useImmediateOffset())
            subRegWithImmediate();
        else
            sub();
        break;
    case MOV_HI:
        moveHigh();
        break;
    case MVN:
        moveN();
        break;
    case LDRPC:
        loadRegPCRelative();
        break;
    case STRSP:
        storeRegSPRelative();
        break;
    case STR:
        str();
        break;
    case LDR:
        ldr();
        break;
    case STRH:
        storeHalfReg();
        break;
    case LDRH:
        loadHalfReg();
        break;
    case ADD:
        add();
        break;
    case ADDSP:
        addSP();
        break;
    case B:
        branch();
        break;
    case B_COND:
        condBranch();
        break;
    case B_WORD:
        longBranch();
        break;
    case BX:
        branchExchange();
        break;
    case PUSH:
        push();
        break;
    case POP:
        pop();
        break;
    case LSL:
        shiftLeft();
        break;
    case LSR:
        shiftRight();
        break;
    case TST:
        test();
        break;
    case ORR:
        logicalOR();
        break;
    case CMP:
        compare();
        break;
    case CMP_HI:
        compareHigh();
        break;
    case LDM:
        loadMultipleReg();
        break;
    case STM:
        storeMultipleReg();
        break;
    case MOV_REG_ADDRESS:
        moveRegAddress();
        break;
    default:
        cout << "Undefined: " << decodedInstruction->toString() << endl;
        exit(FAILED_TO_EXECUTE);
    }
}

void ThumbCpu::step(){
    decode();
    execute();
    if (!reg->isThumbMode()){
        decodedInstruction = new ThumbInstruction();
        return;
    }
    reg->step();
}

bool ThumbCpu::canExecute(Condition cond){
    return reg->canExecute(cond.value);
}

void ThumbCpu::loadRegPCRelative(){
    int regBValue = (reg->getReg(PC)+HALFWORD_SIZE) & ~2;
    int address = regBValue + decodedInstruction->getImmediate()*4;
    int data = mem->read32(address);
    DEBUG_OUT<<"address = "<<address<<", data = "<< data << endl;
    reg->setReg(decodedInstruction->getRegDest(), data);
}

void ThumbCpu::storeReg(){
    ThumbSDT* sdt = (ThumbSDT*) decodedInstruction;
    int regBValue = reg->getReg(sdt->getRegBase());
    int regOValue = reg->getReg(sdt->getRegOffset());
    int data = reg->getReg(sdt->getRegDest());
    int address = regBValue + regOValue;
    DEBUG_OUT<<"address = "<< address <<", data = "<< data << endl;
    mem->write32(address, data);
}

void ThumbCpu::storeRegSPRelative(){
    int regBValue = reg->getReg(SP);
    int address = regBValue + decodedInstruction->getImmediate()*4;
    int data = reg->getReg(decodedInstruction->getRegDest());
    DEBUG_OUT<<"address = "<< address <<", data = "<< data << endl;
    mem->write32(address, data);
}

void ThumbCpu::addSP(){
    int regSValue = reg->getReg(SP);
    int imm = decodedInstruction->getImmediate();
    int result = regSValue + imm;
    DEBUG_OUT<<"result SP = "<< result << endl;
    reg->setReg(SP, result);
}

void ThumbCpu::longBranch(){
    ThumbLongBranch* b = (ThumbLongBranch*) decodedInstruction;
    if (b->isFirstOpcode()){
        int jumpAddress = reg->getReg(PC);
        jumpAddress += b->getImmediate();
        reg->setReg(LR, jumpAddress);
        DEBUG_OUT << "ADDR = "<< jumpAddress <<endl;
    } else{
        int jumpAddress = reg->getReg(LR) + b->getImmediate();
        reg->setReg(LR, reg->getReg(PC)|1); // To return in THUMB mode
        reg->setReg(PC, jumpAddress);
        DEBUG_OUT << "ADDR = "<< jumpAddress << ", linkADDR = "<< reg->getReg(LR) <<endl;
    }
}

void ThumbCpu::add(){
    if (decodedInstruction->useImmediateOffset())
        addRegWithImmediate();
    else
        addImmediate();
}

void ThumbCpu::str(){
    if (decodedInstruction->useImmediateOffset())
        storeImmediateOffset();
    else
        storeReg();
}

void ThumbCpu::ldr(){
    if(decodedInstruction->useImmediateOffset())
        loadImmediateOffset();
    else
        // loadReg();
        exit(PENDING_CODE);
}

void ThumbCpu::compare(){
    if(decodedInstruction->useImmediate())
        compareImmediate();
    else
        compareReg();
}
