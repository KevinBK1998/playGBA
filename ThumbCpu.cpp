#include "Registers.h"
#include "Memory.h"
#include "ThumbInstructions/Instruction.cpp"
#include "ThumbInstructions/ALU.h"

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
public:
    ThumbCpu(Registers* registers, Memory* memory){
        reg = registers;
        mem = memory;
    }
    void decode(){
        int currentPC = reg->getPC();
        int opcode = mem->read16(currentPC);
        if (((opcode>>13) & 0b111)== 1)
            decodedInstruction = ThumbALU::decodeALU(opcode);
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
