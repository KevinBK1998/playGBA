package com.kbkapps.playgba.cpu.opcodes.thumb;

import com.kbkapps.playgba.cpu.constants.Instructions;

public class ExchangingBranch extends OpCode {
    int regS;

    public ExchangingBranch(byte regSource) {
        super(Instructions.BX);
        regS = regSource;
    }

    public int getRegSource() {
        return regS;
    }

    @Override
    public String toString() {
        return instruction.toString() + " " + getRegName(regS);
    }
}
