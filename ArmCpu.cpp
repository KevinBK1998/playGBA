#include <iostream>
#include <string.h>
#include "Registers.cpp"
#include "ArmInstruction.cpp"
#include "Memory.cpp"

using namespace std;

const int N = 0x80000000;
const int Z = 0x40000000;

class ArmCpu
{
private:
    Registers* reg;
    Memory mem;
    ArmInstruction decodedInstruction;
    int fetchedPC;
    void branch();
    void compare();
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
        if (((opcode>>26) & 0b11) == 0b00){
            cout << "Incomplete ALU{cond} Rd, Rn, Op2 = " << hex << opcode << endl;
            int op1 = (opcode >> 16) & 0xF;
            int imm = opcode & 0xFF;
            decodedInstruction = ArmInstruction(CMP, imm, op1);
        }
        else if (((opcode>>25) & 0b111) == 0b101){
            cout << "Incomplete B{LX} {cond} label = " << hex << opcode << endl;
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
        cout << "Debug Decoded: " << hex << decodedInstruction.toString() << endl;
        switch (decodedInstruction.getOpcode())
        {
        case B:
            branch();
            break;
        case CMP:
            compare();
            break;
        default:
            cout << "Undefined: " << decodedInstruction.toString() << endl;
            exit(FAILED_TO_EXECUTE);
        }
    }

    void status(){
        cout << "PC: " << hex << showbase << reg->getPC() << endl;
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

void ArmCpu::compare(){
    int before = reg->getReg(decodedInstruction.getRegN());
    int immediate = decodedInstruction.getImmediate();
    int result = before - immediate;
    cout<<"result = "<< hex << result << endl;
    int flags = 0;
    if (((result >> 31) & 1) != 0)
        flags |= N;
    if (((int) result) == 0)
        flags |= Z;
    reg->setCPSR(flags);
}
