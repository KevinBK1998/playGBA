#ifndef ARM_ALU_H
#define ARM_ALU_H

#include <iostream>
#include "../FailureCodes.h"
#include "Instruction.h"
using namespace std;


int rotateRight(int data, int shift){
    return (data>>shift) | (data << (32-shift));
}

class ALU : public ArmInstruction
{
private:
public:
    ALU():ArmInstruction(){}
    ALU(Opcode opcode, int imm, char op1): ArmInstruction(opcode, op1, imm){}
    ALU(Condition cond, Opcode opcode, int imm, char op1): ArmInstruction(cond, opcode, op1, imm){}
    ALU(Opcode opcode, char destReg, int imm): ArmInstruction(opcode, imm, destReg){}
    ALU(Condition cond, Opcode opcode, char destReg, int imm): ArmInstruction(cond, opcode, imm, destReg){}
    ALU(Opcode opcode, char destReg, char op1, int imm): ArmInstruction(opcode, op1, destReg, imm){}
    ALU(Condition cond, Opcode opcode, char destReg, char op1, int imm): ArmInstruction(cond, opcode, op1, destReg, imm){}
    ALU(Condition cond, Opcode opcode, int imm, char op1, int maskFlags): ArmInstruction(cond, opcode, op1, imm){}

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
            return new ALU(cond, TEQ, imm, rN);
        case 0xA:
            return new ALU(cond, CMP, imm, rN);
        case 0xC:
            return new ALU(cond, ORR, rDest, rN, imm);
        case 0xD:
            return new ALU(cond, MOV, rDest, imm);
        case 0xE:;
        }
        cout << "Incomplete ALU Rd, Rn, Op2 = " << opcode << ", Funcode = " << ((opcode >> 21) & 0xF) << endl;
        exit(FAILED_TO_DECODE);
    }

    bool shouldUseImmediate(){
        return true;
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
