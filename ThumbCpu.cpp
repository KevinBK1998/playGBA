#include "Registers.h"
#include "Memory.h"
#include "ThumbInstructions/Instruction.cpp"
#include "ThumbInstructions/ALU.h"
#include "ThumbInstructions/SingleDataTransfer.h"
#include "ThumbInstructions/MultipleDataTransfer.h"
#include "ThumbInstructions/Branch.h"

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
    void loadReg();
    void storeReg();
    void add();
    void addSP();
    void branch();
    void push();
    void branchExchange();
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
        if (((opcode>>8) & 0xFF) == 0xB0)
            decodedInstruction = ThumbALU::decode(opcode, true);
        else if (((opcode>>10) & 0b111111)== 0b10001)
            decodedInstruction = ThumbBranch::decode(opcode, true);
        else if (((opcode>>11) & 0b11111)== 0b1001)
            decodedInstruction = ThumbSDT::decode(opcode, true);
        else if (((opcode>>11) & 0b11111)== 0b11)
            decodedInstruction = ThumbALU::decode(opcode);
        else if (((opcode>>12) & 0b1111)== 0b101)
            decodedInstruction = ThumbSDT::decode(opcode);
        else if (((opcode>>12) & 0b1111)== 0b1011)
            decodedInstruction = ThumbMDT::decode(opcode);
        else if (((opcode>>12) & 0b1111)== 0b1101)
            decodedInstruction = ThumbBranch::decode(opcode);
        else if (((opcode>>13) & 0b111)== 1)
            decodedInstruction = ThumbALU::decode(opcode, false);
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
        case LDR:
            loadReg();
            break;
        case STR:
            storeReg();
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
        case BX:
            branchExchange();
            break;
        case PUSH:
            push();
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
    ThumbALU* alu = (ThumbALU*) decodedInstruction;
    int immediate = alu->getImmediate();
    reg->setReg(alu->getRegDest(), immediate);
    cout<<"result = "<< hex << immediate << endl;
    reg->setFlags(generateFlags(immediate));
}

void ThumbCpu::loadReg(){
    ThumbSDT* sdt = (ThumbSDT*) decodedInstruction;
    int regBValue = reg->getReg(sdt->getRegBase()) - HALFWORD_SIZE; // -HALF_WORD_SIZE Required because LoadFromPC considers r15 = PC + WORD_SIZE
    int address = regBValue + sdt->getImmediate()*4;
    int data;
    data = mem->read32(address);
    cout<<"data = "<< data << endl;
    reg->setReg(sdt->getRegDest(), data);
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
