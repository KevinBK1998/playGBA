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
public:
    ArmAluInstruction():ArmInstruction(){}
    ArmAluInstruction(Opcode opcode, char op1, int imm): ArmInstruction(opcode, op1, imm){}
    ArmAluInstruction(Opcode opcode, int imm, char destReg): ArmInstruction(opcode, imm, destReg){}
    static ArmAluInstruction decodeALU(int opcode){
        char rN = (opcode >> 16) & 0xF;
        char rDest = (opcode >> 12) & 0xF;
        int shift = (opcode >> 8) & 0xF;
        int imm = opcode & 0xFF;
        imm = rotateRight(imm, shift*2);
        switch ((opcode >> 21) & 0xF)
        {
        case 0x9:
            return ArmAluInstruction(TEQ, rN, imm);
        case 0xA:
            return ArmAluInstruction(CMP, rN, imm);
        case 0xD:
            return ArmAluInstruction(MOV, imm, rDest);
        default:
            exit(FAILED_TO_DECODE_ALU);
        }
    }
    ~ArmAluInstruction(){}
};

#endif
