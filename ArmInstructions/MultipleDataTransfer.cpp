#include <sstream>
#include "../ArmCpu.h"

class MultipleDataTransfer : public ArmInstruction
{
private:
    bool preFlag;
    bool addFlag;
    bool writeBackFlag;
    int regList;
    string getSuffix(){
        stringstream stream;
        if (addFlag)
            stream<<"I";
        else
            stream<<"D";

        if (preFlag)
            stream<<"B";
        else
            stream<<"A";

        if (writeBackFlag)
            stream<<"!";
        else
            stream<<"";

        return stream.str();
    }
public:
    MultipleDataTransfer(Condition cond, Opcode operation, char op1, int flags, int list):ArmInstruction(cond, operation, op1){
        preFlag = ((flags >> 3) & 1) != 0;
        addFlag = ((flags >> 2) & 1) != 0;
        bool specialFlag = ((flags >> 1) & 1) != 0;
        if (specialFlag) exit(FAILED_TO_DECODE);
        writeBackFlag = (flags & 1) != 0;
        regList = list;
    }

    static MultipleDataTransfer* decode(int opcode){
        Condition cond = Condition((opcode >> 28) & 0xF);
        DEBUG_OUT << "MultipleDataTransfer = " << opcode << endl;
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
            if (getRegN() == 13 && getSuffix()=="IA!")
                stream<<"POP";
            else
                stream<<"LDM"<<getSuffix()<<"[R"<< getRegN() <<"]";
            break;
        case STM:
            if (getRegN() == 13 && getSuffix()=="DB!")
                stream<<"PUSH"; 
            else
                stream<<"STM"<<getSuffix()<<"[R"<< getRegN() <<"]";
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

    //Calculate lowest address before loading
    if(!mdt->shouldAddOffset()){
        for (int i = 0; i < 16; i++){
            if (list & 1){
                address-=WORD_SIZE;
            }
            list>>=1;
        }
        if (mdt->shouldWriteBack())
            reg->setReg(mdt->getRegN(),address);
        list = mdt->getRegList();
    }

    for (int i = 0; i < 16; i++){
        if (list & 1){
            if (!(mdt->addBeforeTransfer() ^ mdt->shouldAddOffset()))
                address+=WORD_SIZE;
            data = mem->read32(address);
            DEBUG_OUT<<"address = "<<address<<", R"<<dec<<i<<hex<<" = "<< data << endl;
            reg->setReg(i, data);
            if (mdt->addBeforeTransfer() ^ mdt->shouldAddOffset())
                address+=WORD_SIZE;
        }
        list>>=1;
    }
    if (mdt->shouldAddOffset() && mdt->shouldWriteBack())
        reg->setReg(mdt->getRegN(),address);
}

void ArmCpu::storeMultipleReg(){
    MultipleDataTransfer* mdt = (MultipleDataTransfer*) decodedInstruction;
    int address = reg->getReg(mdt->getRegN());
    int data;
    int list = mdt->getRegList();

    //Calculate lowest address before storing
    if(!mdt->shouldAddOffset()){
        for (int i = 0; i < 16; i++){
            if (list & 1){
                address-=WORD_SIZE;
            }
            list>>=1;
        }
        if (mdt->shouldWriteBack())
            reg->setReg(mdt->getRegN(),address);
        list = mdt->getRegList();
    }

    for (int i = 0; i < 16; i++){
        if (list & 1){
            if (!(mdt->addBeforeTransfer() ^ mdt->shouldAddOffset()))
                address+=WORD_SIZE;
            data = reg->getReg(i);
            DEBUG_OUT<<"address = "<<address<<", R"<<dec<<i<<hex<<" = "<< data << endl;
            mem->write32(address, data);
            if (mdt->addBeforeTransfer() ^ mdt->shouldAddOffset())
                address+=WORD_SIZE;
        }
        list>>=1;
    }
    if(mdt->shouldAddOffset() && mdt->shouldWriteBack())
        reg->setReg(mdt->getRegN(),address);
}
