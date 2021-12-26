package com.kbkapps.playgba.cpu;

import com.kbkapps.playgba.cpu.constants.Instructions;
import com.kbkapps.playgba.cpu.opcodes.thumb.OpCode;

import static com.kbkapps.playgba.cpu.ArmV3Cpu.N;
import static com.kbkapps.playgba.cpu.ArmV3Cpu.Z;

public class ThumbCpu {

    private final Registers reg;

    public ThumbCpu(Registers registers, GbaMemory gbaMemory) {
        reg = registers;
    }

    public void execute(OpCode opcode) throws UndefinedOpcodeException {
        if (opcode == null) {
            System.out.println("tNOP");
            return;
        } else System.out.println("Executing: " + opcode);
        if (opcode.getInstruction() == Instructions.MOV) {
            int regD = opcode.getRegDest();
            reg.setReg(regD, opcode.getImmediate());
            setFlags(reg.getReg(regD));
        }
    }

    private void setFlags(long result) {
        int flags = 0;
        if (((result >> 31) & 1) != 0)
            flags |= N;
        if (((int) result) == 0)
            flags |= Z;
//        if ((result>>32)>0)
//            flags |= C;
//        if ((result >> 32) > 0)
//            flags |= V;
        reg.setFlags(flags);
    }
}
