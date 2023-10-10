#include <sstream>
#include "../ArmCpu.h"

using namespace std;

class ALUReg: public ArmInstruction
{
private:
public:
    static ALUReg* decode(int opcode){
        Condition cond = Condition((opcode >> 28) & 0xF);
        char op = (opcode >> 21) & 0xF;
        char rDest = (opcode >> 12) & 0xF;
        switch (op)
        {
        case 8:;
        default:
            cout << "ALUReg = " << unsigned(op) << endl;
            exit(FAILED_TO_DECODE);
            break;
        //FIX LSL AND LSR FIRST
        }
    }

    string toString(){
        stringstream stream;
        switch (getOpcode())
        {
        default:
            cout << "ALUReg = " << hex << getOpcode() << endl;
            exit(FAILED_DECODED_TO_STRING);
        }
        return "Undefined";
    }
};
