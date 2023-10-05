#include <iostream>
#include <string.h>
#include "Registers.cpp"
#include "ArmInstruction.cpp"
#include "Memory.cpp"

using namespace std;

class ArmCpu
{
private:
    Registers* reg;
    Memory mem;
    ArmInstruction decodedInstruction;
    int fetchedPC;
    void branch();
public:
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
        if (((opcode>>25) & 0b111) == 0b101){
            cout << "Incomplete B{LX} {cond} label = 0x" << hex << opcode << endl;
            int imm = opcode & 0xFFF;
            decodedInstruction = ArmInstruction(B, imm);
        }
        else{
            cout << "Undefined: 0x" << hex << opcode << endl;
            exit(FAILED_TO_DECODE);
        }
    }

    void execute(){
        if (decodedInstruction.getOpcode() == NOT_INITIALISED)
            return;
        cout << "Debug Decoded:" << hex << decodedInstruction.toString() << endl;
        if (decodedInstruction.getOpcode() == B)
            branch();
        else{
            cout << "Undefined: " << decodedInstruction.toString() << endl;
            exit(FAILED_TO_DECODE);
        }
    }

    void status(){
        cout << "PC: 0x" << hex << reg->getPC() << endl;
    }

    void step(){
        status();
        execute();
        decode();
        reg->step();
    }
};

void ArmCpu::branch(){
    reg->branch(decodedInstruction.getImmediate());
}
