#include <iostream>
#include "FailureCodes.h"
#include "CommonDS.h"
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
    return reg15;
}

void Registers::setPC(int imm){
    reg15 = imm + getStepAmount();
}

uint32_t Registers::getReg(int index){
    if (index < 13)
        return unbankedReg[index];
    if (index < 15){
        return bankedReg[getBankedIndex(index)];
    }
    if (index == 15)
        return reg15 + getStepAmount();
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

void Registers::setFlags(char mask, int data){
    int changeMask = mask<<24;
    int protectMask = ~changeMask;
    currentStatusReg &= protectMask;
    currentStatusReg |= (data & changeMask);
}

int Registers::getSPSR(){
    return savedStatusReg[mode];
}

void Registers::setSPSR(int data){
    savedStatusReg[mode] = data;
}

void Registers::branch(int imm){
    if (thumbMode)
        reg15 += (imm+1)*HALFWORD_SIZE;
    else
        reg15 += (imm+1)*WORD_SIZE;
}

bool Registers::isThumbMode(){
    return thumbMode;
}

void Registers::exchange(int address){
    bool prevThumbMode = thumbMode;
    thumbMode=(address&1);
    if(thumbMode){
        reg15 = address & ~1;
        if (prevThumbMode)
            reg15 -= HALFWORD_SIZE;
        DEBUG_OUT << "\nPC.t = "<< reg15 <<endl;
    }
    else{
        reg15 = address & ~2;
        if (!prevThumbMode)
            reg15 -= WORD_SIZE;
        DEBUG_OUT << "\nPC.a = "<< reg15 <<endl;
    }
}

void Registers::step(){
    reg15 += getStepAmount();
}

void Registers::status(){
    DEBUG_OUT<<"\nMODE: "<<(thumbMode?"THUMB":"ARM")<<"("<<getMode()<<")"<<" \tPC: "<<getPC()<<endl;
    DEBUG_OUT<<"R0: "<<unbankedReg[0]<<"\tR1: "<<unbankedReg[1]<<"\tR2: "<< unbankedReg[2]<<"\tR3: "<< unbankedReg[3]<< endl;
    DEBUG_OUT<<"R4: "<<unbankedReg[4]<<"\tR5: "<<unbankedReg[5]<<"\tR6: "<< unbankedReg[6]<<"\tR7: "<< unbankedReg[7]<< endl;
    DEBUG_OUT<<"R8: "<<getReg(8)<<"\tR9: "<<getReg(9)<<"\tR10: "<< getReg(10)<<"\tR11: "<< getReg(11)<< endl;
    DEBUG_OUT<<"R12: "<<getReg(12)<<"\tR13(SP): "<<getReg(SP)<<"\tR14(LR): "<< getReg(LR)<<"\tR15(PC+"<<noshowbase<<getStepAmount()<<showbase<<"): "<< getReg(PC)<< endl;
    DEBUG_OUT<<"CPSR: "<<currentStatusReg<< "\tSPSR:"<<getSPSR()<< endl;
}

bool Registers::canExecute(int cond){
    switch (cond)
    {
    case EQ:
        return (currentStatusReg & Z) != 0;
    case NE:
        return (currentStatusReg & Z) == 0;
    case CC:
        return (currentStatusReg & C) == 0;
    case MI:
        return (currentStatusReg & N) != 0;
    case PL:
        return (currentStatusReg & N) == 0;
    case VS:
        return (currentStatusReg & V) != 0;
    case GE:
        return canExecute(MI) == canExecute(VS);
    case LT:
        return canExecute(MI) != canExecute(VS);
    case ALWAYS:
        if(thumbMode) exit(FAILED_TO_DECODE);
        return true;
    default:
        cout << "Cond Undefined: " << Condition(cond).toString() << endl;
        exit(FAILED_TO_EXECUTE);
    }
    // if (cond == CS) return (currentStatusReg & 0x20_00_00_00) != 0;
    // else if (cond == VC) return (currentStatusReg & 0x10_00_00_00) == 0;
    // else if (cond == HI) return canExecute(CS) && canExecute(NE);
    // else if (cond == LS) return canExecute(CC) || canExecute(EQ);
    // else if (cond == GT) return canExecute(NE) && canExecute(GE);
    // else if (cond == LE) return canExecute(EQ) || canExecute(LT);
    return false;
}

string Condition::toString(){
    switch (value)
    {
    case EQ:
        return "{eq}";
    case NE:
        return "{ne}";
    case CC:
        return "{cc}";
    case PL:
        return "{pl}";
    case GE:
        return "{ge}";
    case LT:
        return "{lt}";
    case ALWAYS:
        return "";
    default:
        cout << "COND = " << value << endl;
        exit(FAILED_DECODED_TO_STRING);
    }
    return "Never";
}
