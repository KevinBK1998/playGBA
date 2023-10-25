#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include "Registers.cpp"
#include "Memory.cpp"
#include "GPU.cpp"
#include "APU.cpp"
#include "ArmCpu.cpp"
#include "ThumbCpu.cpp"

using namespace std;
int timePassed=0;
int DEBUG_AFTER_CYCLES=0;
int DEBUG_AFTER_PC=0x0;
Registers reg;

void status(){
    DEBUG_OUT<<"Time passed: "<<dec<<timePassed<<" cycles"<<hex<< endl;
}

int main(int argc, char *args[]){
    DEBUG_AFTER_CYCLES=760871;
    // DEBUG_AFTER_PC=0x90c;
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "playGBA - debug mode");
    GPU gpu(&window);
    Memory mem(&gpu);
    if (argc > 1)
        mem = Memory(&gpu, args[argc - 1]);
    ArmCpu cpu = ArmCpu(&reg, &mem);
    ThumbCpu thumbCpu = ThumbCpu(&reg, &mem);
    cout<<"Starting up CPU"<<hex<<showbase<<endl;
    // while(timePassed < 602841){
    // while(true){
    while(timePassed < DEBUG_AFTER_CYCLES + 500){
        status();
        reg.status();
        if (reg.isThumbMode())
            thumbCpu.step();
        else
            cpu.step();
        gpu.step();
        timePassed++;
        if(timePassed==DEBUG_AFTER_CYCLES || reg.getPC()==DEBUG_AFTER_PC) DEBUG_LOGS=true;
    }
    mem.dump(reg.getReg(SP));
    cout<<"Time to start HBLANK"<< endl;
    return 0;
}
