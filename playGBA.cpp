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

void status(Registers reg){
    cout << "PC: " << hex << showbase << reg.getPC() << " \tMODE: " << (reg.isThumbMode()?"THUMB":"ARM") << endl;
}

int main(int argc, char *args[]){
    if (argc > 1)
        load(args[argc - 1]);
    Registers reg;
    ArmCpu cpu = ArmCpu(&reg);
    while(true){
        status(reg);
        cpu.step();
    }
    return 0;
}
