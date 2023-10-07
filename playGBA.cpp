#include <iostream>
#include <fstream>
#include "Registers.h"
#include "ArmCpu.cpp"

using namespace std;

void load(char *fileName){
    ifstream fin(fileName);
    if (!fin)
    {
        cout << "Error Opening ROM\n";
        // exit(-1);
    }
    // cout << "Loaded Rom:" << fileName << endl;
}

void status(Registers reg, ArmCpu cpu){
    cout<<hex<<showbase<<"\nMODE: "<<(reg.isThumbMode()?"THUMB":"ARM")<<" \tPC: "<<reg.getPC()<<endl;
    cout<<"R0: "<<reg.getReg(0)<<"\tR1: "<<reg.getReg(1)<<"\tR2: "<< reg.getReg(2)<<"\tR3: "<< reg.getReg(3)<< endl;
    cout<<"R4: "<<reg.getReg(4)<<"\tR5: "<<reg.getReg(5)<<"\tR6: "<< reg.getReg(6)<<"\tR7: "<< reg.getReg(7)<< endl;
    cout<<"R8: "<<reg.getReg(8)<<"\tR9: "<<reg.getReg(9)<<"\tR10: "<< reg.getReg(10)<<"\tR11: "<< reg.getReg(11)<< endl;
    cout<<"R12: "<<reg.getReg(12)<<"\tR13(SP): "<<reg.getReg(13)<<"\tR14(LR): "<< reg.getReg(14)<<"\tR15(PC+4): "<< reg.getReg(15)<< endl;
    cout<<"CPSR: "<<reg.getCPSR()<< "\tTime passed:"<<dec<<cpu.time<<" cycles"<<hex<< endl;
}

int main(int argc, char *args[]){
    if (argc > 1)
        load(args[argc - 1]);
    Registers reg;
    ArmCpu cpu = ArmCpu(&reg);
    while(true){
        status(reg, cpu);
        cpu.step();
    }
    return 0;
}
