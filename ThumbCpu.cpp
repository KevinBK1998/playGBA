#include "Registers.h"
#include "Memory.h"
#include "ThumbInstructions/Instruction.cpp"
#include "ThumbInstructions/ALU.h"
#include "ThumbInstructions/SingleDataTransfer.h"

class ThumbCpu{
    Registers* reg;
    Memory* mem;
    ThumbInstruction* decodedInstruction = new ThumbInstruction();
    int generateFlags(int result){
        int flags = 0;
        if (((result >> 31) & 1) != 0)
            flags |= N;
        if (((int) result) == 0)
            flags |= Z;
        return flags;
    }
    void move();
    void loadReg();
    void storeReg();
public:
    ThumbCpu(Registers* registers, Memory* memory){
        reg = registers;
        mem = memory;
    }
    void decode(){
        int currentPC = reg->getPC();
        int opcode = mem->read16(currentPC);
        if (((opcode>>11) & 0b11111)== 0b1001)
            decodedInstruction = ThumbSDT::decode(opcode, true);
        else if (((opcode>>12) & 0b1111)== 0b101)
            decodedInstruction = ThumbSDT::decode(opcode);
        else if (((opcode>>13) & 0b111)== 1)
            decodedInstruction = ThumbALU::decode(opcode);
        else{
            cout << "Undecoded Opcode: " << opcode << endl;
            exit(FAILED_TO_DECODE);
        }
    }
    void execute(){
        if (decodedInstruction->getOpcode() == NOT_INITIALISED)
            return;
        cout << "Debug Execute: " << decodedInstruction->toString() << endl;
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
        default:
            cout << "Undefined: " << decodedInstruction->toString() << endl;
            exit(FAILED_TO_EXECUTE);
        }
    }

    void step(){
        execute();
        decode();
        reg->step();
    }
};

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
