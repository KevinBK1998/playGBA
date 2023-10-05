#ifndef REGISTERS_H
#define REGISTERS_H

class Registers
{
private:
    // Registers 0-12
    int unbankedReg[13];
    // Registers 13(SP) and 14(LR)
    int bankedReg[10];
    int savedStatusReg[5];
    int currentStatusReg;
    int reg15 = 4;
public:
    Registers(/* args */);
    ~Registers();
    int getPC();
    void setPC(int);
    void branch(int);
    void step();
};

#endif
