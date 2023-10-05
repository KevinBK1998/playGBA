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

void Registers::branch(int imm){
    reg15 = reg15 + imm*4;
}

void Registers::step(){
    reg15 += 4;
}
