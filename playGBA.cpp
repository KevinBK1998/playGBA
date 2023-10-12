#include <iostream>
#include <fstream>
#include "Registers.cpp"
#include "Memory.cpp"
#include "GPU.cpp"
#include "ArmCpu.cpp"
#include "ThumbCpu.cpp"

using namespace std;
int timePassed=0;
int DEBUG_AFTER=457;
Memory mem;
Registers reg;

void status(){
    DEBUG_OUT<<"Time passed:"<<dec<<timePassed<<" cycles"<<hex<< endl;
}

int main(int argc, char *args[]){
    if (argc > 1)
        mem = Memory(args[argc - 1]);
    ArmCpu cpu = ArmCpu(&reg, &mem);
    ThumbCpu thumbCpu = ThumbCpu(&reg, &mem);
    cout<<"Starting up CPU"<<hex<< endl;
    while(timePassed < 1000){
        status();
        reg.status();
        if (reg.isThumbMode())
            thumbCpu.step();
        else
            cpu.step();
        timePassed++;
        if(timePassed==DEBUG_AFTER) DEBUG_LOGS=true;
    }
    cout<<"Time to start HBLANK"<< endl;
    return 0;
}
