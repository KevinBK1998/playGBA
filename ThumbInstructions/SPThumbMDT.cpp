#include <sstream>
#include "../ThumbCpu.h"

class SPThumbMDT: public ThumbInstruction
{
private:
    bool linkFlag;
    char regList;
public:
    SPThumbMDT(Opcode opcode, bool flag, char list): ThumbInstruction(opcode){
        linkFlag = flag;
        regList = list;
    }

    static SPThumbMDT* decode(int opcode){
        Opcode op = ((opcode>>11)&1)? POP : PUSH;
        if (((opcode>>9)&0b11) != 0b10){
            cout<<"Not MDT"<< endl;
            exit(PENDING_CODE);
        }
        bool flag = (opcode>>8)&1;
        char rList = opcode & 0xFF;
        return new SPThumbMDT(op, flag, rList);
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

void ThumbCpu::push(){
    SPThumbMDT* mdt = (SPThumbMDT*) decodedInstruction;
    int address = reg->getReg(SP);
    int data;
    int list = mdt->getRegList();

    // Calculate lowest address first
    for (int i = 0; i < 8; i++){
        if (list & 1) address-=WORD_SIZE;
        list>>=1;
    }
    if (mdt->handleLinkFlag()) address-=WORD_SIZE;

    reg->setReg(SP, address);
    list = mdt->getRegList();

    for (int i = 0; i < 8; i++){
        if (list & 1){
            data = reg->getReg(i);
            DEBUG_OUT<<"address = "<<address<<", R"<<dec<<i<<hex<<" = "<< data << endl;
            mem->write32(address, data);
            address+=WORD_SIZE;
        }
        list>>=1;
    }
    if (mdt->handleLinkFlag()){
        data = reg->getReg(LR);
        DEBUG_OUT<<"address = "<<address<<", R"<<dec<<LR<<hex<<" = "<< data << endl;
        mem->write32(address, data);
    }
}

void ThumbCpu::pop(){
    SPThumbMDT* mdt = (SPThumbMDT*) decodedInstruction;
    int address = reg->getReg(SP);
    int data;
    int list = mdt->getRegList();

    for (int i = 0; i < 8; i++){
        if (list & 1){
            data = mem->read32(address);
            DEBUG_OUT<<"address = "<<address<<", R"<<dec<<i<<hex<<" = "<< data << endl;
            reg->setReg(i, data);
            address+=WORD_SIZE;
        }
        list>>=1;
    }
    if (mdt->handleLinkFlag()){
        data = mem->read32(address);
        DEBUG_OUT<<"address = "<<address<<", R"<<dec<<PC<<hex<<" = "<< data << endl;
        reg->setReg(PC, data);
    }
    reg->setReg(SP, address);
}
