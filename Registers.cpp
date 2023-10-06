#include "FailureCodes.h"
#include "Registers.h"

Registers::Registers(/* args */)
{
}

Registers::~Registers()
{
}

int Registers::getPC(){
    return reg15 - 4;
}

void Registers::setPC(int imm){
    reg15 = imm + 4;
}

int Registers::getReg(int index){
    if (index < 13)
        return unbankedReg[index];
    if (index < 15){
        return bankedReg[getBankedIndex(index)];
    }
    if (index == 15)
        return reg15;
}

void Registers::setReg(char index, int data){
    if (index < 13)
        unbankedReg[index] = data;
    else if (index < 15)
        bankedReg[getBankedIndex(index)] = data;
    else if (index == 15)
        reg15 = data;
}

int Registers::getCPSR(){
    return currentStatusReg;
}

void Registers::setCPSR(int data){
    currentStatusReg = data;
    setControlBits();
}

int Registers::getSPSR(){
    return savedStatusReg[mode];
}

void Registers::setSPSR(int data){
    savedStatusReg[mode] = data;
}

void Registers::branch(int imm){
    reg15 = reg15 + imm*4 + 4;
}

bool Registers::isThumbMode(){
    return thumbMode;
}

void Registers::step(){
    reg15 += 4;
}
