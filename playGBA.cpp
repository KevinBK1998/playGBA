#include <iostream>
#include <fstream>
#include "Registers.cpp"
#include "Memory.cpp"
#include "GPU.cpp"
#include "ArmCpu.cpp"
#include "ThumbCpu.cpp"

using namespace std;
int timePassed=0;
Memory mem;
Registers reg;

void status(){
    cout<<"Time passed:"<<dec<<timePassed<<" cycles"<<hex<< endl;
}

int main(int argc, char *args[]){
    if (argc > 1)
        mem = Memory(args[argc - 1]);
    ArmCpu cpu = ArmCpu(&reg, &mem);
    ThumbCpu thumbCpu = ThumbCpu(&reg, &mem);
    while(timePassed < 1000){
        status();
        reg.status();
        if (reg.isThumbMode())
            thumbCpu.step();
        else
            cpu.step();
        timePassed++;
    }
    cout<<"Time to start HBLANK"<< endl;
    return 0;
}
