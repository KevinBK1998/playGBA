package com.kbkapps.playgba.cpu;

import com.kbkapps.playgba.cpu.opcodes.OpCode;

public class ThumbCpu {

    private final Registers reg;

    public ThumbCpu(Registers registers, GbaMemory gbaMemory) {
        reg = registers;
    }

    public void execute(OpCode opcodeDecoded) throws UndefinedOpcodeException {

    }
}
