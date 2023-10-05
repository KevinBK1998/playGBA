#include <iostream>
#include <fstream>
#include "Memory.cpp"

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
    Memory mem;
    cout << "Test:0x" << hex << unsigned(mem.read8(0)) << endl;
    cout << "Test:0x" << unsigned(mem.read8(1)) << endl;
    cout << "Test:0x" << unsigned(mem.read8(2)) << endl;
    cout << "Test:0x" << unsigned(mem.read8(3)) << endl;
    cout << "Test:0x" << mem.read32(0) << endl;
    return 0;
}
