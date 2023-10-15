#include <sstream>
#include "../ThumbCpu.h"

class ThumbMDT: public ThumbInstruction
{
private:
    char regBase;
    char regList;
public:
    ThumbMDT(Opcode opcode, char rB, char list): ThumbInstruction(opcode){
        regBase = rB;
        regList = list;
    }

    static ThumbMDT* decode(int opcode){
        Opcode op = ((opcode>>11)&1)? LDM : STM;
        char rB = (opcode>>8) & 7;
        char rList = opcode & 0xFF;
        return new ThumbMDT(op, rB, rList);
    }

    char getRegList(){
        return regList;
    }

    bool getRegBase(){
        return regBase;
    }

    string toString(){
        stringstream stream;
        string linkString;
        switch (getOpcode())
        {
        case STM:
            stream << "STMIA";
            break;
        case LDM:
            stream << "LDMIA";
            break;
        default:
            cout << "OPCODE = " << hex << getOpcode() << endl;
            exit(FAILED_DECODED_TO_STRING);
            return "Undefined";
        }
        stream <<" R"<<unsigned(regBase)<<"!, {";
        int list = regList;
        for (int i = 0; i < 8; i++){
            if (list & 1)
                stream<<"R"<<i<<",";
            list>>=1;
        }
        stream<<"\b}";
        return stream.str();
    }
};

void ThumbCpu::storeMultipleReg(){
    ThumbMDT* mdt = (ThumbMDT*) decodedInstruction;
    int address = reg->getReg(mdt->getRegBase());
    int list = mdt->getRegList();

    for (int i = 0; i < 8; i++){
        if (list & 1){
            int data = reg->getReg(i);
            DEBUG_OUT<<"address = "<<address<<", R"<<dec<<i<<hex<<" = "<< data << endl;
            mem->write32(address, data);
            address+=WORD_SIZE;
        }
        list>>=1;
    }
    reg->setReg(mdt->getRegBase(), address);
}

void ThumbCpu::loadMultipleReg(){
    ThumbMDT* mdt = (ThumbMDT*) decodedInstruction;
    int address = reg->getReg(mdt->getRegBase());
    int list = mdt->getRegList();

    for (int i = 0; i < 8; i++){
        if (list & 1){
            int data = mem->read32(address);
            DEBUG_OUT<<"address = "<<address<<", R"<<dec<<i<<hex<<" = "<< data << endl;
            reg->setReg(i, data);
            address+=WORD_SIZE;
        }
        list>>=1;
    }
    reg->setReg(mdt->getRegBase(), address);
}
