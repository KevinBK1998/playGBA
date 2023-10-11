#include <sstream>
#include "../ThumbCpu.h"

using namespace std;

class AddRegImmediate: public ThumbInstruction
{
private:
    char regSource;
    char regNum;
    bool useImm;
public:
    AddRegImmediate(Opcode opcode, char regD, char regS, char regN):ThumbInstruction(opcode, regD){
        regSource = regS;
        regNum = regN;
        useImm = false;
    }
    AddRegImmediate(Opcode opcode, char regD, char regS, int imm):ThumbInstruction(opcode, regD, imm){
        regSource = regS;
        useImm = true;
    }

    static AddRegImmediate* decode(int opcode){
        char rS = (opcode>>3) & 0b111;
        char rD = opcode & 0b111;
        char op = (opcode>>9) & 0x3;
        int imm = (opcode>>6) & 0b111;
        char rN = imm;
        switch (op)
        {
        case 0:
            return new AddRegImmediate(ADD, rD, rS, rN);
        case 2:
            return new AddRegImmediate(ADD, rD, rS, imm);
        case 3:
            return new AddRegImmediate(SUB, rD, rS, imm);
        default:
            cout << "AddRegImmediate = " << unsigned(op) << endl;
            exit(FAILED_TO_DECODE);
            break;
        }
    }

    char getRegSource(){
        return regSource;
    }

    char getRegNum(){
        return regNum;
    }

    bool shouldUseImmediate(){
        return useImm;
    }

    bool useImmediateOffset(){
        return true;
    }

    string toString(){
        stringstream stream;
        switch (getOpcode())
        {
        case ADD:
            stream<<"ADD";
            break;
        case SUB:
            stream<<"SUB";
            break;
        default:
            cout << "AddRegImmediate = " << hex << getOpcode() << endl;
            exit(FAILED_DECODED_TO_STRING);
        }
        stream<<" R"<<unsigned(getRegDest())<<", R"<<unsigned(regSource);
        if(useImm)
            stream<<showbase<<hex<<", "<<getImmediate();
        else 
            stream<<", R"<<unsigned(regNum);
        return stream.str();
    }
};

void ThumbCpu::addRegWithImmediate(){
    AddRegImmediate* alu = (AddRegImmediate*) decodedInstruction;
    int op1 = reg->getReg(alu->getRegSource());
    bool signS= op1 > 0;
    int op2 = alu->shouldUseImmediate()? alu->getImmediate(): reg->getReg(alu->getRegNum());
    bool signI= op2 > 0;
    int result = op1 + op2;
    bool signR= result > 0;
    int flags = generateFlags(result);
    if (signS == signI && signR!=signS){
        cout<<"signedFlags = "<< signS <<","<< signI<<","<<signR << endl;
        exit(PENDING_CODE);
    }
    cout<<"result = "<<result<<", flags = "<<flags<<endl;
    reg->setReg(alu->getRegDest(), result);
    reg->setFlags(flags);
}

void ThumbCpu::subRegWithImmediate(){
    AddRegImmediate* alu = (AddRegImmediate*) decodedInstruction;
    int op1 = reg->getReg(alu->getRegSource());
    bool signS= op1 > 0;
    int op2 = alu->shouldUseImmediate()? alu->getImmediate(): reg->getReg(alu->getRegNum());
    bool signI= op2 > 0;
    int result = op1 - op2;
    bool signR= result > 0;
    int flags = generateFlags(result);
    if (signS != signI && signR!=signS){
        cout<<"signedFlags = "<< signS <<","<< signI<<","<<signR << endl;
        exit(PENDING_CODE);
    }
    cout<<"result = "<<result<<", flags = "<<flags<<endl;
    reg->setReg(alu->getRegDest(), result);
    reg->setFlags(flags);
}
