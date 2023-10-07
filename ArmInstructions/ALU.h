#ifndef ARM_ALU_H
#define ARM_ALU_H

#include <iostream>
#include "FailureCodes.h"
#include "Instruction.h"
using namespace std;


int rotateRight(int data, int shift){
    return (data>>shift) | (data << (32-shift));
}

class ALU : public ArmInstruction
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
    ALU():ArmInstruction(){}
    ALU(Opcode opcode, int imm, char op1): ArmInstruction(opcode, op1, imm){}
    ALU(Condition cond, Opcode opcode, int imm, char op1): ArmInstruction(cond, opcode, op1, imm){}
    ALU(Opcode opcode, char destReg, int imm): ArmInstruction(opcode, imm, destReg){}
    ALU(Condition cond, Opcode opcode, char destReg, int imm): ArmInstruction(cond, opcode, imm, destReg){}
    ALU(Opcode opcode, char destReg, char op1, int imm): ArmInstruction(opcode, op1, destReg, imm){}
    ALU(Condition cond, Opcode opcode, char destReg, char op1, int imm): ArmInstruction(cond, opcode, op1, destReg, imm){}
    ALU(Condition cond, Opcode opcode, int imm, char op1, int maskFlags): ArmInstruction(cond, opcode, op1, imm){
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

    static ALU* decodeALU(int opcode){
        Condition cond = Condition((opcode >> 28) & 0xF);
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
        case 0:
            return new ALU(cond, AND, rDest, rN, imm);
        case 2:
            return new ALU(cond, SUB, rDest, rN, imm);
        case 4:
            return new ALU(cond, ADD, rDest, rN, imm);
        case 9:
            if (((opcode >> 25) & 0b1) != 0)
                return new ALU(cond, TEQ, imm, rN);
            break;
        case 0xA:
            return new ALU(cond, CMP, imm, rN);
        case 0xC:
            return new ALU(cond, ORR, rDest, rN, imm);
        case 0xD:
            return new ALU(cond, MOV, rDest, imm);
        }
        if (((opcode >> 23) & 0b11) == 0b10) {
            bool psr = ((opcode >> 22) & 0b1) != 0;
            if((opcode >> 21) & 0b1){
                char rM = opcode & 0xF;
                int maskFlags = (opcode >> 16) & 0xF;
                return new ALU(cond, MSR, psr, rM, maskFlags);
            }
            else
                return new ALU(cond, MRS, rDest, psr);
        }
        cout << "Incomplete ALU Rd, Rn, Op2 = " << opcode << ", Funcode = " << ((opcode >> 21) & 0xF) << endl;
        exit(FAILED_TO_DECODE_ALU);
    }

    string toString(){
        stringstream stream;
        switch (getOpcode())
        {
        case AND:
            stream << showbase << "AND"<<getCondition()<<" R" << getRegDest() << ", R" << getRegN() << hex << ", " << getImmediate();
            return stream.str();
        case SUB:
            stream << showbase << "SUB"<<getCondition()<<" R" << getRegDest() << ", R" << getRegN() << hex << ", " << getImmediate();
            return stream.str();
        case ADD:
            stream << showbase << "ADD"<<getCondition()<<" R" << getRegDest() << ", R" << getRegN() << hex << ", " << getImmediate();
            return stream.str();
        case MRS:
            stream << showbase << "MRS"<<getCondition()<<" R" << getRegDest() << hex << ", " << (getImmediate()?'S':'C') << "PSR";
            return stream.str();
        case MSR:
            stream << showbase << "MSR"<<getCondition()<<" " << (getImmediate()?'S':'C') << "PSR_" << getMaskString() << ", R" << getRegN();
            return stream.str();
        case TEQ:
            stream << showbase << "TEQ"<<getCondition()<<" R" << getRegN() << hex << ", " << getImmediate();
            return stream.str();
        case CMP:
            stream << showbase << "CMP"<<getCondition()<<" R" << getRegN() << hex << ", " << getImmediate();
            return stream.str();
        case ORR:
            stream << showbase << "ORR"<<getCondition()<<" R" << getRegDest() << ", R" << getRegN() << hex << ", " << getImmediate();
            return stream.str();
        case MOV:
            stream << showbase << "MOV"<<getCondition()<<" R" << getRegDest() << hex << ", " << getImmediate();
            return stream.str();

        default:
            cout << "ALU = " << hex << getOpcode() << endl;
            exit(FAILED_DECODED_TO_STRING);
        }
        return "Undefined";
    }
    ~ALU(){}
};

#endif
