#ifndef ARM_ALU_INSTR_H
#define ARM_ALU_INSTR_H

#include <iostream>
#include "FailureCodes.h"
#include "ArmInstruction.h"
using namespace std;


int rotateRight(int data, int shift){
    return (data>>shift) | (data << (32-shift));
}

class ArmAluInstruction : public ArmInstruction
{
private:
    bool isSavedPsr;
    char regM;
    int mask;
    string getMaskString(){
        stringstream stream;
        if ((mask >> 3) & 1)
            stream << "f";
        if ((mask >> 2) & 1)
            stream << "s";
        if ((mask >> 1) & 1)
            stream << "x";
        if (mask & 1)
            stream << "c";
        return stream.str();
    }
public:
    ArmAluInstruction():ArmInstruction(){}
    ArmAluInstruction(Opcode opcode, int imm, char op1): ArmInstruction(opcode, op1, imm){}
    ArmAluInstruction(Opcode opcode, char destReg, int imm): ArmInstruction(opcode, imm, destReg){}
    ArmAluInstruction(Opcode opcode, char destReg, char op1, int imm): ArmInstruction(opcode, op1, destReg, imm){}
    ArmAluInstruction(Opcode opcode, int imm, char op1, int maskFlags): ArmInstruction(opcode, op1, imm){
        mask = maskFlags;
    }
    
    int getMask(){
        int expMask = 0;
        for (int i = 3; i >= 0; i--) {
            expMask = expMask << 8;
            if ((mask >> i) & 1)
                expMask += 0xFF;
        }
        return expMask;
    }

    static ArmAluInstruction* decodeALU(int opcode){
        char rN = (opcode >> 16) & 0xF;
        // cout << "rN = " << dec << unsigned(rN) << endl;
        char rDest = (opcode >> 12) & 0xF;
        // cout << "rD = " << unsigned(rDest) << endl;
        int shift = (opcode >> 8) & 0xF;
        int imm = opcode & 0xFF;
        imm = rotateRight(imm, shift*2);
        // cout << "imm = " << hex << imm << endl;
        switch ((opcode >> 21) & 0xF)
        {
        case 0x9:
            if (((opcode >> 25) & 0b1) != 0)
                return new ArmAluInstruction(TEQ, imm, rN);
            break;
        case 0xA:
            return new ArmAluInstruction(CMP, imm, rN);
        case 0xC:
            return new ArmAluInstruction(ORR, rDest, rN, imm);
        case 0xD:
            return new ArmAluInstruction(MOV, rDest, imm);
        }
        if (((opcode >> 23) & 0b11) == 0b10) {
            bool psr = ((opcode >> 22) & 0b1) != 0;
            if((opcode >> 21) & 0b1){
                char rM = opcode & 0xF;
                int maskFlags = (opcode >> 16) & 0xF;
                return new ArmAluInstruction(MSR, psr, rM, maskFlags);
            }
            else
                return new ArmAluInstruction(MRS, rDest, psr);
        }
        cout << "ALU = " << hex << ((opcode >> 21) & 0xF) << endl;
        exit(FAILED_TO_DECODE_ALU);
    }

    string toString(){
        stringstream stream;
        switch (getOpcode())
        {
        case MRS:
            stream << showbase << "MRS R" << getRegDest() << hex << ", " << (getImmediate()?'S':'C') << "PSR";
            return stream.str();
        case MSR:
            stream << showbase << "MSR " << (getImmediate()?'S':'C') << "PSR_" << getMaskString() << ", R" << getRegN();
            return stream.str();
        case TEQ:
            stream << showbase << "TEQ R" << getRegN() << hex << ", " << getImmediate();
            return stream.str();
        case CMP:
            stream << showbase << "CMP R" << getRegN() << hex << ", " << getImmediate();
            return stream.str();
        case ORR:
            stream << showbase << "ORR R" << getRegDest() << ", R" << getRegN() << hex << ", " << getImmediate();
            return stream.str();

        case MOV:
            stream << showbase << "MOV R" << getRegDest() << hex << ", " << getImmediate();
            return stream.str();

        default:
            cout << "ALU = " << hex << getOpcode() << endl;
            exit(FAILED_DECODED_TO_STRING);
        }
        return "Undefined";
    }
    ~ArmAluInstruction(){}
};

#endif
