#include <iostream>
#include "FailureCodes.h"
#include "Registers.h"

using namespace std;

Registers::Registers(/* args */)
{
}

Registers::~Registers()
{
}

void Registers::setControlBits() {
    irqEnable = (currentStatusReg & 0x80) == 0;
    thumbMode = (currentStatusReg & 0x20) != 0;
    int modeMeta = currentStatusReg & 0x1F;
    switch (modeMeta) {
        case 0:
        case 16:
            mode = SYSTEM_OR_USER;
            privilegedUser = false;
            break;
        case 2:
        case 18:
            mode = INTERRUPT_REQUEST;
            break;
        case 3:
        case 19:
            mode = SUPERVISOR;
            break;
        case 23:
            mode = ABORT;
            break;
        case 27:
            mode = UNDEFINED;
            break;
        case 31:
            mode = SYSTEM_OR_USER;
            privilegedUser = true;
            break;
        default:
            exit(PENDING_CODE);
    }
}

int Registers::getStepAmount() {
    return thumbMode ? HALFWORD_SIZE : WORD_SIZE;
}

int Registers::getPC(){
    return reg15 - getStepAmount();
}

void Registers::setPC(int imm){
    reg15 = imm + getStepAmount();
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

void Registers::setFlags(int data){
    currentStatusReg &= 0xFFFFFF;
    currentStatusReg |= (data & 0xFF000000);
}

int Registers::getSPSR(){
    return savedStatusReg[mode];
}

void Registers::setSPSR(int data){
    savedStatusReg[mode] = data;
}

void Registers::branch(int imm){
    int jump = imm+1;
    if (thumbMode)
        jump *= HALFWORD_SIZE;
    else
        jump *= WORD_SIZE;
    reg15 += jump;
}

bool Registers::isThumbMode(){
    return thumbMode;
}

void Registers::exchange(int address){
    thumbMode=(address&1);
    if(thumbMode){
        reg15 = address^1 + getStepAmount();
        cout << "\nPC.t = "<< reg15 <<endl;
    }
    else{
        reg15 = (address&(~0b11)) - getStepAmount();
        cout << "\nPC.a = "<< reg15 <<endl;
    }
}

void Registers::step(){
    reg15 += getStepAmount();
}

void Registers::status(){
    cout<<"\nMODE: "<<(thumbMode?"THUMB":"ARM")<<"("<<getMode()<<")"<<" \tPC: "<<getPC()<<endl;
    cout<<"R0: "<<unbankedReg[0]<<"\tR1: "<<unbankedReg[1]<<"\tR2: "<< unbankedReg[2]<<"\tR3: "<< unbankedReg[3]<< endl;
    cout<<"R4: "<<unbankedReg[4]<<"\tR5: "<<unbankedReg[5]<<"\tR6: "<< unbankedReg[6]<<"\tR7: "<< unbankedReg[7]<< endl;
    cout<<"R8: "<<getReg(8)<<"\tR9: "<<getReg(9)<<"\tR10: "<< getReg(10)<<"\tR11: "<< getReg(11)<< endl;
    cout<<"R12: "<<getReg(12)<<"\tR13(SP): "<<getReg(SP)<<"\tR14(LR): "<< getReg(LR)<<"\tR15(PC+"<<noshowbase<<getStepAmount()<<showbase<<"): "<< reg15<< endl;
    cout<<"CPSR: "<<currentStatusReg<< "\tSPSR:"<<getSPSR()<< endl;
}
