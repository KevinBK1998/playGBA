#ifndef THUMB_ADD_IMM_H
#define THUMB_ADD_IMM_H

#include <iostream>
#include <sstream>
#include "Instruction.h"
#include "../FailureCodes.h"

using namespace std;

class AddIMM: public ThumbInstruction
{
private:
    char regSource;
public:
    AddIMM(Opcode opcode, char regD, char regS, int imm):ThumbInstruction(opcode, regD, imm){
        regSource = regS;
    }

    static AddIMM* decode(int opcode){
        char rS = (opcode>>3) & 0b111;
        char rD = opcode & 0b111;
        char op = (opcode>>9) & 0x3;
        uint8_t imm = (opcode>>6) & 0b111;
        switch (op)
        {
        case 2:
            return new AddIMM(ADD, rD, rS, imm);
        default:
            cout << "AddIMM = " << unsigned(op) << endl;
            exit(FAILED_TO_DECODE);
            break;
        }
    }

    char getRegSource(){
        return regSource;
    }

    string toString(){
        stringstream stream;
        switch (getOpcode())
        {
        case ADD:
            stream << showbase << "ADD R"<< unsigned(getRegDest())<<", R" << unsigned(regSource) << hex << ", " << getImmediate();
            break;
        default:
            cout << "AddIMM = " << hex << getOpcode() << endl;
            exit(FAILED_DECODED_TO_STRING);
        }
        return stream.str();
    }
};

#endif
