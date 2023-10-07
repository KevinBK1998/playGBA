#include <iostream>
#include <fstream>
#include "Registers.h"
#include "ArmCpu.cpp"

using namespace std;
int timePassed=0;

void load(char *fileName){
    ifstream fin(fileName);
    if (!fin)
    {
        cout << showbase << hex << "Error Opening ROM" << endl;
        // exit(-1);
    }
    // cout << "Loaded Rom:" << fileName << endl;
}

void status(){
    cout<<"Time passed:"<<dec<<timePassed<<" cycles"<<hex<< endl;
}

int main(int argc, char *args[]){
    if (argc > 1)
        load(args[argc - 1]);
    Registers reg;
    ArmCpu cpu = ArmCpu(&reg);
    while(timePassed < 1000){
        status();
        reg.status();
        cpu.step();
        timePassed++;
    }
    return 0;
}
