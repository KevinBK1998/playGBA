package com.kbkapps.playgba.cpu.opcodes;

import com.kbkapps.playgba.cpu.UndefinedOpcodeException;
import com.kbkapps.playgba.cpu.constants.Flags;
import com.kbkapps.playgba.cpu.constants.Instructions;

import static com.kbkapps.playgba.cpu.constants.Flags.values;

public class Branch extends OpCode {
    int offset;

    public Branch(Instructions opcode, Flags cond, int jumpBy) {
        super(opcode, cond, true);
        instruction = opcode;
        offset = jumpBy;
        condition = cond;
    }

    public static Branch decodeOpcode(int opcodeEncoded) throws UndefinedOpcodeException {
        Flags cond = values()[(opcodeEncoded >> 28) & 0xF];
        if (((opcodeEncoded >> 24) & 0x1) == 0b0) {//Branch
            Instructions opcode = Instructions.B;
            int jumpBy = opcodeEncoded & 0x00_FF_FF_FF;
            if ((jumpBy & 0x80_00_00) != 0)
                jumpBy |= 0xFF_00_00_00;
            return new Branch(opcode, cond, jumpBy);
        }
        System.out.println("cond = " + cond);
        System.out.println("rest of opcodeEncoded(ALU) = " + Integer.toUnsignedString(opcodeEncoded & 0xF_FF_FF_FF, 16));
        throw new UndefinedOpcodeException(opcodeEncoded);
    }

    public int getOffset() {
        return offset;
    }

    @Override
    public String toString() {
        if (instruction == Instructions.B)
            return condition.toString() + " " + instruction.toString() + " " + Integer.toString(offset, 16);
        return super.toString();
    }
}
