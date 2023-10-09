#include "Registers.h"
#include "Memory.h"
#include "ThumbInstructions/Instruction.cpp"
#include "ThumbInstructions/ALU.h"
#include "ThumbInstructions/AddSP.h"
#include "ThumbInstructions/ALUImmediate.h"
#include "ThumbInstructions/ShiftedALU.h"
#include "ThumbInstructions/SingleDataTransfer.h"
#include "ThumbInstructions/LoadPCRelative.h"
#include "ThumbInstructions/SDTRelativeSP.h"
#include "ThumbInstructions/MultipleDataTransfer.h"
#include "ThumbInstructions/Branch.h"
#include "ThumbInstructions/LongBranch.h"
#include "ThumbInstructions/SDTImmediate.h"

class ThumbCpu{
    Registers* reg;
    Memory* mem;
    ThumbInstruction* decodedInstruction = new ThumbInstruction();
    int generateFlags(int result){
        int flags = 0;
        if (result < 0)
            flags |= N;
        if (result == 0)
            flags |= Z;
        return flags;
    }
    void move();
    void moveN();
    void loadRegPCRelative();
    void storeReg();
    void storeHalfReg();
    void storeRegSPRelative();
    void add();
    void addSP();
    void branch();
    void longBranch();
    void push();
    void branchExchange();
    void shiftLeft();
    bool canExecute(int);
    bool canExecute(Condition);
public:
    ThumbCpu(Registers* registers, Memory* memory){
        reg = registers;
        mem = memory;
    }
    void decode(){
        // cout<<"DEBUG"<< endl;
        int currentPC = reg->getPC();
        int opcode = mem->read16(currentPC);
        cout <<showbase<< "Debug Opcode: " << opcode << endl;
        if (((opcode>>8) & 0xFF) == 0xB0)
            decodedInstruction = AddSP::decode(opcode);
        else if (((opcode>>10) & 0x3F)== 0b010000)
            decodedInstruction = ThumbALU::decode(opcode);
        else if (((opcode>>10) & 0x3F)== 0b10001)
            decodedInstruction = ThumbBranch::decode(opcode, true);
        else if (((opcode>>11) & 0x1F)== 0b1001)
            decodedInstruction = LoadPCRelative::decode(opcode);
        else if (((opcode>>11) & 0x1F)== 0b11)
            decodedInstruction = ThumbALU::decode(opcode);
        else if (((opcode>>12) & 0b1111)== 0b101)
            decodedInstruction = ThumbSDT::decode(opcode);
        else if (((opcode>>12) & 0b1111)== 0b1000)
            decodedInstruction = SDTThumbIMM::decode(opcode);
        else if (((opcode>>12) & 0b1111)== 0b1001)
            decodedInstruction = SDTRelativeSP::decode(opcode);
        else if (((opcode>>12) & 0b1111)== 0b1011)
            decodedInstruction = ThumbMDT::decode(opcode);
        else if (((opcode>>12) & 0b1111)== 0b1101)
            decodedInstruction = ThumbBranch::decode(opcode);
        else if (((opcode>>12) & 0b1111)== 0b1111)
            decodedInstruction = ThumbLongBranch::decode(opcode);
        else if (((opcode>>13) & 0b111)== 0)
            decodedInstruction = ShiftedALU::decode(opcode);
        else if (((opcode>>13) & 0b111)== 1)
            decodedInstruction = ALUThumbIMM::decode(opcode);
        else{
            cout << "Undecoded Opcode: " << opcode << endl;
            exit(FAILED_TO_DECODE);
        }
    }
    void execute(){
        if (decodedInstruction->getOpcode() == NOT_INITIALISED){
            cout << "No cached Instruction, skipping" << endl;
            return;
        }
        cout <<showbase<< "Debug Execute: " << decodedInstruction->toString() << endl;
        switch (decodedInstruction->getOpcode())
        {
        case MOV:
            move();
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
            storeReg();
            break;
        case STRH:
            storeHalfReg();
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
        case B_WORD:
            longBranch();
            break;
        case BX:
            branchExchange();
            break;
        case PUSH:
            push();
            break;
        case LSL:
            shiftLeft();
            break;
        default:
            cout << "Undefined: " << decodedInstruction->toString() << endl;
            exit(FAILED_TO_EXECUTE);
        }
    }

    void step(){
        execute();
        if (!reg->isThumbMode()){
            decodedInstruction = new ThumbInstruction();
            return;
        }
        decode();
        reg->step();
    }
};

bool ThumbCpu::canExecute(int cond){
    int status = reg->getCPSR();
    switch (cond)
    {
    case EQ:
        return (status & Z) != 0;
    case NE:
        return (status & Z) == 0;
    case MI:
        return (status & N) != 0;
    case VS:
        return (status & V) != 0;
    case LT:
        return canExecute(MI) != canExecute(VS);
    default:
        cout << "Cond Undefined: " << Condition(cond).toString() << endl;
        exit(FAILED_TO_EXECUTE);
    }
    return false;
}

bool ThumbCpu::canExecute(Condition cond){
    return canExecute(cond.value);
}

void ThumbCpu::move(){
    int immediate = decodedInstruction->getImmediate();
    reg->setReg(decodedInstruction->getRegDest(), immediate);
    cout<<"result = "<< hex << immediate << endl;
    reg->setFlags(generateFlags(immediate));
}

void ThumbCpu::moveN(){
    ThumbALU* alu = (ThumbALU*) decodedInstruction;
    int data = ~reg->getReg(alu->getRegSource());
    reg->setReg(alu->getRegDest(), data);
    cout<<"result = "<< hex << data << endl;
    reg->setFlags(generateFlags(data));
}

void ThumbCpu::loadRegPCRelative(){
    int regBValue = reg->getPC();
    if(regBValue == 0x9ca) regBValue+=HALFWORD_SIZE; // Workaround for now, need to fix
    int address = regBValue + decodedInstruction->getImmediate()*4;
    int data = mem->read32(address);
    cout<<"address = "<<address<<", data = "<< data << endl;
    reg->setReg(decodedInstruction->getRegDest(), data);
}

void ThumbCpu::storeReg(){
    ThumbSDT* sdt = (ThumbSDT*) decodedInstruction;
    int regBValue = reg->getReg(sdt->getRegBase());
    int regOValue = reg->getReg(sdt->getRegOffset());
    int data = reg->getReg(sdt->getRegDest());
    cout<<"data = "<< data << endl;
    int address = regBValue + regOValue;
    cout<<"address = "<< address << endl;
    mem->write32(address, data);
}

void ThumbCpu::storeHalfReg(){
    SDTThumbIMM* sdt = (SDTThumbIMM*) decodedInstruction;
    int base = reg->getReg(sdt->getRegBase());
    int offset = sdt->getImmediate()<<1;
    int data = reg->getReg(sdt->getRegDest());
    int address = base + offset;
    cout<<"address = "<< address <<", data = "<< data << endl;
    mem->write16(address, data);
}

void ThumbCpu::storeRegSPRelative(){
    int regBValue = reg->getReg(SP);
    int address = regBValue + decodedInstruction->getImmediate()*4;
    int data = reg->getReg(decodedInstruction->getRegDest());
    cout<<"address = "<< address <<", data = "<< data << endl;
    mem->write32(address, data);
}

void ThumbCpu::add(){
    ThumbALU* alu = (ThumbALU*) decodedInstruction;
    int regSValue = reg->getReg(alu->getRegSource());
    bool signS= regSValue > 0;
    int imm = alu->getImmediate();
    bool signI= imm > 0;
    int result = regSValue + imm;
    bool signR= result > 0;
    if (signS == signI && signR!=signI){
        cout<<"signedFlags = "<< signS <<","<< signI<<","<<signR << endl;
        exit(PENDING_CODE);
    }
    cout<<"result = "<< result << endl;
    reg->setReg(alu->getRegDest(), result);
    cout<<"flags = "<< generateFlags(result) << endl;
    reg->setFlags(generateFlags(result));
}

void ThumbCpu::shiftLeft(){
    ShiftedALU* alu = (ShiftedALU*) decodedInstruction;
    int regSValue = reg->getReg(alu->getRegSource());
    int imm = alu->getImmediate();
    int result = regSValue << imm;
    cout<<"result = "<< result << endl;
    reg->setReg(alu->getRegDest(), result);
    cout<<"flags = "<< generateFlags(result) << endl;
    reg->setFlags(generateFlags(result));
}

void ThumbCpu::addSP(){
    int regSValue = reg->getReg(SP);
    bool signS= regSValue > 0;
    int imm = decodedInstruction->getImmediate() << 2;
    bool signI= imm > 0;
    int result = regSValue + imm;
    bool signR= result > 0;
    if (signS == signI && signR!=signI){
        cout<<"signedFlags = "<< signS <<","<< signI<<","<<signR << endl;
        exit(PENDING_CODE);
    }
    cout<<"result SP = "<< result << endl;
    reg->setReg(SP, result);
}

void ThumbCpu::branch(){
    ThumbBranch* b = (ThumbBranch*) decodedInstruction;
    if (canExecute(b->getPreCheck())){
        reg->branch(b->getImmediate());
    } else cout << "Skipping, condition failed" << endl;
}

void ThumbCpu::longBranch(){
    ThumbLongBranch* b = (ThumbLongBranch*) decodedInstruction;
    if (b->isFirstOpcode()){
        int jumpAddress = reg->getReg(PC) + HALFWORD_SIZE;
        jumpAddress += b->getImmediate();
        reg->setReg(LR, jumpAddress);
        cout << "ADDR = "<< jumpAddress <<endl;
    } else{
        int jumpAddress = reg->getReg(LR) + b->getImmediate();
        reg->setReg(LR, reg->getReg(PC) + HALFWORD_SIZE);
        reg->setReg(PC, jumpAddress);
        cout << "ADDR = "<< jumpAddress << ", linkADDR = "<< reg->getReg(LR) <<endl;
    }
}

void ThumbCpu::branchExchange(){
    ThumbBranch* b = (ThumbBranch*) decodedInstruction;
    int jumpTo = reg->getReg(b->getRegSource());
    cout << "ADDR = "<< jumpTo <<endl;
    reg->exchange(jumpTo);
}

void ThumbCpu::push(){
    ThumbMDT* mdt = (ThumbMDT*) decodedInstruction;
    int address = reg->getReg(SP);
    int data;
    int list = mdt->getRegList();
    for (int i = 0; i < 8; i++){
        if (list & 1){
            address-=WORD_SIZE;
            data = reg->getReg(i);
            cout<<"address = "<<address<<", R"<<dec<<i<<hex<<" = "<< data << endl;
            mem->write32(address, data);
        }
        list>>=1;
    }
    if (mdt->handleLinkFlag()){
        address-=WORD_SIZE;
        data = reg->getReg(LR);
        cout<<"address = "<<address<<", R"<<dec<<LR<<hex<<" = "<< data << endl;
        mem->write32(address, data);
    }
    reg->setReg(SP, address);
}
