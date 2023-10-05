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

int main(int argc, char *args[]){
    if (argc > 1)
        load(args[argc - 1]);
    Registers reg;
    ArmCpu cpu = ArmCpu(&reg);
    cpu.step();
    cpu.step();
    cpu.step();
    cpu.step();
    return 0;
}
