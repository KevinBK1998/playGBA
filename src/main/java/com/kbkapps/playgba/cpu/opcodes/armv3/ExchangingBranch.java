package com.kbkapps.playgba.cpu.opcodes.armv3;

import com.kbkapps.playgba.cpu.UndefinedOpcodeException;
import com.kbkapps.playgba.cpu.constants.Flags;
import com.kbkapps.playgba.cpu.constants.Instructions;

import static com.kbkapps.playgba.cpu.constants.Flags.values;

public class ExchangingBranch extends OpCode {
    public ExchangingBranch(Instructions opcode, Flags cond, int regNo) {
        super(opcode, cond, false);
        this.regNo = regNo;
    }

    public static ExchangingBranch decodeOpcode(int opcodeEncoded) throws UndefinedOpcodeException {
        Flags cond = values()[(opcodeEncoded >> 28) & 0xF];
        if (((opcodeEncoded >> 4) & 0xF) == 1)
            return new ExchangingBranch(Instructions.BX, cond, opcodeEncoded & 0xf);
        System.out.println("cond = " + cond);
        System.out.println("rest of opcodeEncoded(ExBranch) = 0x" + Integer.toUnsignedString(opcodeEncoded & 0xFF, 16));
        throw new UndefinedOpcodeException(opcodeEncoded);
    }

    @Override
    public String toString() {
        if (instruction == Instructions.BX)
            return condition.toString() + " " + instruction.toString() + " " + getRegName(regNo);
        return super.toString();
    }
}
