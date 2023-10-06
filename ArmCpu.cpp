#include <iostream>
#include <string.h>
#include "Registers.cpp"
#include "ArmInstruction.cpp"
#include "ArmSdtInstruction.h"
#include "ArmAluInstruction.h"
#include "Memory.cpp"

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
    Memory mem;
    ArmInstruction decodedInstruction;
    int fetchedPC;
    int setFlags(int result){
        int flags = 0;
        if (((result >> 31) & 1) != 0)
            flags |= N;
        if (((int) result) == 0)
            flags |= Z;
        return flags;
    }
    void branch();
    void testXOR();
    void compare();
    void move();
    void loadToReg();
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
            decodedInstruction= ArmAluInstruction::decodeALU(opcode);
        }
        else if (((opcode>>26) & 0b11) == 0b01){
            cout << "Incomplete PLD/STR/LDR{cond}{B}{T} Rd, <Address> = " << hex << opcode << endl;
            if (((opcode >> 20) & 1) != 0){
                int flags = ((opcode >> 22) & 0xF);
                int data = opcode & 0xFFFFF;
                char rN = (opcode >> 16) & 0xF;
                char rDest = (opcode >> 12) & 0xF;
                decodedInstruction=ArmSdtInstruction(LDR, rN, flags, data, rDest);
            }
        }
        else if (((opcode>>25) & 0b111) == 0b101){
            cout << "Incomplete B{LX} {cond} label = " << hex << opcode << endl;
            int imm = opcode & 0xFFF;
            decodedInstruction = ArmInstruction(B, imm);
        }
        else{
            cout << "Undefined Opcode: " << hex << opcode << endl;
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
        case TEQ:
            testXOR();
            break;
        case CMP:
            compare();
            break;
        case MOV:
            move();
            break;
        case LDR:
            loadToReg();
            break;
        default:
            cout << "Undefined: " << decodedInstruction.toString() << endl;
            exit(FAILED_TO_EXECUTE);
        }
    }

    void step(){
        execute();
        decode();
        reg->step();
    }
};

void ArmCpu::branch(){
    reg->branch(decodedInstruction.getImmediate());
}

void ArmCpu::testXOR(){
    int before = reg->getReg(decodedInstruction.getRegN());
    int immediate = decodedInstruction.getImmediate();
    int result = before ^ immediate;
    cout<<"result = "<< hex << result << endl;
    reg->setCPSR(setFlags(result));
}

void ArmCpu::compare(){
    int before = reg->getReg(decodedInstruction.getRegN());
    int immediate = decodedInstruction.getImmediate();
    int result = before - immediate;
    cout<<"result = "<< hex << result << endl;
    reg->setCPSR(setFlags(result));
}

void ArmCpu::move(){
    int immediate = decodedInstruction.getImmediate();
    reg->setReg(decodedInstruction.getRegDest(), immediate);
    cout<<"result = "<< hex << immediate << endl;
    reg->setCPSR(setFlags(immediate));
}

void ArmCpu::loadToReg(){
    int address = 0;
    int regNValue = reg->getReg(decodedInstruction.getRegN());
    address = regNValue + decodedInstruction.getImmediate();
    int data = mem.read8(address);
    cout<<"result = "<< data << endl;
    reg->setReg(decodedInstruction.getRegDest(), data);
}
