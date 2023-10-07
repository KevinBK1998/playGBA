#include <iostream>
#include "ArmInstructions/FailureCodes.h"
#include "Registers.h"

using namespace std;

Registers::Registers(/* args */)
{
}

Registers::~Registers()
{
}

int Registers::getPC(){
    return reg15 - WORD_SIZE;
}

void Registers::setPC(int imm){
    reg15 = imm + WORD_SIZE;
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
    reg15 = reg15 + imm*4 + WORD_SIZE;
}

bool Registers::isThumbMode(){
    return thumbMode;
}

void Registers::step(){
    reg15 += WORD_SIZE;
}

void Registers::status(){
    cout<<"\nMODE: "<<(thumbMode?"THUMB":"ARM")<<" \tPC: "<<getPC()<<endl;
    cout<<"R0: "<<unbankedReg[0]<<"\tR1: "<<unbankedReg[1]<<"\tR2: "<< unbankedReg[2]<<"\tR3: "<< unbankedReg[3]<< endl;
    cout<<"R4: "<<unbankedReg[4]<<"\tR5: "<<unbankedReg[5]<<"\tR6: "<< unbankedReg[6]<<"\tR7: "<< unbankedReg[7]<< endl;
    cout<<"R8: "<<getReg(8)<<"\tR9: "<<getReg(9)<<"\tR10: "<< getReg(10)<<"\tR11: "<< getReg(11)<< endl;
    cout<<"R12: "<<getReg(12)<<"\tR13(SP): "<<getReg(13)<<"\tR14(LR): "<< getReg(14)<<"\tR15(PC+"<<WORD_SIZE<<"): "<< reg15<< endl;
    cout<<"CPSR: "<<currentStatusReg<< "\tSPSR["<<mode<<"]:"<<getSPSR()<< endl;
}
