#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include "Registers.cpp"
#include "Memory.cpp"
#include "GPU.cpp"
#include "APU.cpp"
#include "ArmCpu.cpp"
#include "ThumbCpu.cpp"

const int OSCREEN_WIDTH = 240;
const int OSCREEN_HEIGHT = 160;
const int INITIAL_ZOOM = 2;
const int SCREEN_WIDTH = INITIAL_ZOOM*OSCREEN_WIDTH;
const int SCREEN_HEIGHT = INITIAL_ZOOM*OSCREEN_HEIGHT;

using namespace std;
int timePassed=0;
int DEBUG_AFTER=740;
// int DEBUG_AFTER=34000;
Registers reg;

void status(){
    DEBUG_OUT<<"Time passed:"<<dec<<timePassed<<" cycles"<<hex<< endl;
}

int main(int argc, char *args[]){
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "playGBA - debug mode");
    GPU gpu(&window);
    Memory mem(gpu);
    if (argc > 1)
        mem = Memory(gpu, args[argc - 1]);
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
    mem.dump();
    cout<<"Time to start HBLANK"<< endl;
    return 0;
}
