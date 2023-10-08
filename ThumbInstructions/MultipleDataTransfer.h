#ifndef THUMB_MDT_H
#define THUMB_MDT_H

#include <iostream>
#include <sstream>
#include "Instruction.h"
#include "../FailureCodes.h"

using namespace std;

class ThumbMDT: public ThumbInstruction
{
private:
    bool linkFlag;
    char regList;
public:
    ThumbMDT(Opcode opcode, bool flag, char list): ThumbInstruction(opcode){
        linkFlag = flag;
        regList = list;
    }

    static ThumbMDT* decode(int opcode){
        Opcode op = ((opcode>>11)&1)? POP : PUSH;
        if (((opcode>>9)&0b11) != 0b10){
            cout<<"Not MDT"<< endl;
            exit(PENDING_CODE);
        }
        bool flag = (opcode>>8)&1;
        char rList = opcode & 0xFF;
        return new ThumbMDT(op, flag, rList);
    }

    char getRegList(){
        return regList;
    }

    bool handleLinkFlag(){
        return linkFlag;
    }

    string toString(){
        stringstream stream;
        string linkString;
        switch (getOpcode())
        {
        case PUSH:
            stream << "PUSH";
            linkString = "LR,";
            break;
        case POP:
            stream << "POP";
            linkString = "PC,";
            break;
        default:
            cout << "OPCODE = " << hex << getOpcode() << endl;
            exit(FAILED_DECODED_TO_STRING);
            return "Undefined";
        }
        stream << " {";
        int list = regList;
        for (int i = 0; i < 8; i++){
            if (list & 1)
                stream<<"R"<<i<<",";
            list>>=1;
        }
        if (linkFlag)
            stream << linkString;
        stream<<"\b}";
        return stream.str();
    }
};

#endif
