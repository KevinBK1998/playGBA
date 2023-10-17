#include <sstream>
#include "../ArmCpu.h"

class SingleDataTransfer : public ArmInstruction
{
private:
    bool preFlag;
    bool addFlag;
    bool byteTransfer;
    bool writeBackFlag;
public:
    SingleDataTransfer():ArmInstruction(){}
    ~SingleDataTransfer(){}

    SingleDataTransfer(Condition cond, Opcode operation, char op1, int flags, int imm, char destReg):ArmInstruction(cond, operation, op1, destReg, imm){
        preFlag = ((flags >> 3) & 1) != 0;//P
        addFlag = ((flags >> 2) & 1) != 0;//U
        if(preFlag && !addFlag){
            cout<< "PRE FLAG in SingleDataTransfer: "<< addFlag << endl;
            exit(PENDING_CODE);
        }
        byteTransfer = ((flags>>1) & 1) != 0;
        bool tFlag = (flags & 1) != 0; //T
        if(!preFlag && tFlag){
            cout<< "T FLAG in SingleDataTransfer" << endl;
            exit(PENDING_CODE);
        }
        if (preFlag)
            writeBackFlag = tFlag;
        else writeBackFlag = true;
    }

    static SingleDataTransfer* decodeSDT(int opcode){
        Condition cond = Condition((opcode >> 28) & 0xF);
        if (cond.value == 0xF)
            cout << "Incomplete PLD{T} Rd, <Address> = " << opcode << endl;
        int flags = (opcode >> 21) & 0xF;
        int imm = opcode & 0xFFF;
        char rN = (opcode >> 16) & 0xF;
        char rDest = (opcode >> 12) & 0xF;
        Opcode operation = ((opcode >> 20) & 1)? LDR : STR;
        return new SingleDataTransfer(cond, operation, rN, flags, imm, rDest);
    }

    bool shouldAddOffsetPreTransfer(){
        return preFlag;
    }

    bool shouldWriteBack(){
        return writeBackFlag;
    }

    int addOffset(int address){
        if (addFlag)
            address+=getImmediate();
        else
            address-=getImmediate();
        return address;
    }

    bool isByteTransfer(){
        return byteTransfer;
    }

    string toString(){
        stringstream stream;
        switch (getOpcode())
        {
        case LDR:
            stream << "LDR";
            break;
        case STR:
            stream << "STR";
            break;
        default:
            exit(FAILED_DECODED_TO_STRING);
        }
        stream<< (byteTransfer?"B":"") << getCondition() <<" R" << getRegDest() << ", [R"<< getRegN() << (preFlag?", ":"] ") << (addFlag?"+":"-") << showbase << hex << getImmediate() << (preFlag?"]":"")<<(writeBackFlag?"!":"");
        return stream.str();
    }
};

void ArmCpu::loadReg(){
    SingleDataTransfer* sdt = (SingleDataTransfer*) decodedInstruction;
    int regNValue = reg->getReg(sdt->getRegN());
    if (sdt->getRegN()==PC)
        regNValue+=WORD_SIZE; //Base Register is PC+8
    int address = regNValue;
    if (sdt->shouldAddOffsetPreTransfer())
        address = sdt->addOffset(address);
    int data;
    if (sdt->isByteTransfer())
        data = mem->read8(address);
    else
        data = mem->read32(address);
    DEBUG_OUT<<"address = "<<address<<", data = "<< data << endl;
    if (!sdt->shouldAddOffsetPreTransfer())
        address = sdt->addOffset(address);
    reg->setReg(sdt->getRegDest(), data);
    regNValue = reg->getReg(sdt->getRegN());
    if (sdt->shouldWriteBack())
        reg->setReg(sdt->getRegN(), address);
}

void ArmCpu::storeReg(){
    SingleDataTransfer* sdt = (SingleDataTransfer*) decodedInstruction;
    int regNValue = reg->getReg(sdt->getRegN());
    if (sdt->getRegN()==PC)
        regNValue+=WORD_SIZE; //Base Register is PC+8
    int data = reg->getReg(sdt->getRegDest());
    if (sdt->getRegDest()==PC)
        data+=2*WORD_SIZE; //Dest Register is PC+12
    int address = regNValue;
    if (sdt->shouldAddOffsetPreTransfer())
        address = sdt->addOffset(address);
    if (sdt->isByteTransfer()){
        DEBUG_OUT<<"address = "<<address<<", data = "<< (data & 0xFF) << endl;
        mem->write8(address, data);
    }
    else{
        DEBUG_OUT<<"address = "<<address<<", data = "<< data << endl;
        mem->write32(address, data);
    }
    if (!sdt->shouldAddOffsetPreTransfer())
        address = sdt->addOffset(address);
    if (sdt->shouldWriteBack())
        reg->setReg(sdt->getRegN(), address);
}
