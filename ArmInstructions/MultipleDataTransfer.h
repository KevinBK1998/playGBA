#ifndef ARM_MDT_H
#define ARM_MDT_H

#include <iostream>
#include <sstream>
#include <string>
#include "../FailureCodes.h"
#include "Instruction.h"
using namespace std;

class MultipleDataTransfer : public ArmInstruction
{
private:
    bool preFlag;
    bool addFlag;
    bool writeBackFlag;
    int regList;
public:
    MultipleDataTransfer():ArmInstruction(){}
    ~MultipleDataTransfer(){}

    MultipleDataTransfer(Condition cond, Opcode operation, char op1, int flags, int list):ArmInstruction(cond, operation, op1){
        preFlag = ((flags >> 3) & 1) != 0;
        addFlag = ((flags >> 2) & 1) != 0;
        writeBackFlag = (flags & 1) != 0;
        regList = list;
    }

    static MultipleDataTransfer* decodeMDT(int opcode){
        Condition cond = Condition((opcode >> 28) & 0xF);
        cout << "Incomplete LDM/STM {cond}{amod} Rn{!},<Rlist>{^} = " << opcode << endl;
        int flags = (opcode >> 21) & 0xF;
        int imm = opcode & 0xFFF;
        char rN = (opcode >> 16) & 0xF;
        int rList = opcode & 0xFFFF;
        Opcode operation = ((opcode >> 20) & 1)? LDM : STM;
        return new MultipleDataTransfer(cond, operation, rN, flags, rList);
    }

    int getRegList(){
        return regList;
    }

    bool shouldWriteBack(){
        return writeBackFlag;
    }

    bool shouldAddOffset(){
        return addFlag;
    }

    bool addBeforeTransfer(){
        return preFlag;
    }

    string toString(){
        stringstream stream;
        switch (getOpcode())
        {
        case LDM:
            if (getRegN() == 13)
                stream<<"POP";
            else
                stream<<"LDM[R"<< getRegN() <<"]";
            break;
        case STM:
            if (getRegN() == 13)
                stream<<"PUSH"; 
            else
                stream<<"STM[R"<< getRegN() <<"]";
            break;
        }
        stream << getCondition() << " {";
        int list = regList;
        for (int i = 0; i < 16; i++){
            if (list & 1)
                stream<<"R"<<i<<",";
            list>>=1;
        }
        stream<<"\b}";
        return stream.str();
    }
};

#endif
