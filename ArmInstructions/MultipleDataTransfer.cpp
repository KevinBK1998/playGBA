#include <sstream>
#include "../ArmCpu.h"

class MultipleDataTransfer : public ArmInstruction
{
private:
    bool preFlag;
    bool addFlag;
    bool writeBackFlag;
    int regList;
public:
    MultipleDataTransfer(Condition cond, Opcode operation, char op1, int flags, int list):ArmInstruction(cond, operation, op1){
        preFlag = ((flags >> 3) & 1) != 0;
        addFlag = ((flags >> 2) & 1) != 0;
        writeBackFlag = (flags & 1) != 0;
        regList = list;
    }

    static MultipleDataTransfer* decode(int opcode){
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

void ArmCpu::loadMultipleReg(){
    MultipleDataTransfer* mdt = (MultipleDataTransfer*) decodedInstruction;
    int address = reg->getReg(mdt->getRegN());
    int data;
    int list = mdt->getRegList();
    int offset = mdt->shouldAddOffset()? WORD_SIZE: -WORD_SIZE;
    for (int i = 0; i < 16; i++){
        if (list & 1){
            if (mdt->addBeforeTransfer())
                address+=offset;
            data = mem->read32(address);
            cout<<"address = "<<address<<", R"<<dec<<i<<hex<<" = "<< data << endl;
            reg->setReg(i, data);
            if (!mdt->addBeforeTransfer())
                address+=offset;
        }
        list>>=1;
    }
    if (mdt->shouldWriteBack())
        reg->setReg(mdt->getRegN(),address);
}

void ArmCpu::storeMultipleReg(){
    MultipleDataTransfer* mdt = (MultipleDataTransfer*) decodedInstruction;
    int address = reg->getReg(mdt->getRegN());
    int data;
    int list = mdt->getRegList();
    int offset = mdt->shouldAddOffset()? WORD_SIZE: -WORD_SIZE;
    for (int i = 15; i >= 0; i--){
        if (list & 0x8000){
            if (mdt->addBeforeTransfer())
                address+=offset;
            data = reg->getReg(i);
            cout<<"address = "<<address<<", R"<<dec<<i<<hex<<" = "<< data << endl;
            mem->write32(address, data);
            if (!mdt->addBeforeTransfer())
                address+=offset;
        }
        list<<=1;
    }
    if (mdt->shouldWriteBack())
        reg->setReg(mdt->getRegN(),address);
}