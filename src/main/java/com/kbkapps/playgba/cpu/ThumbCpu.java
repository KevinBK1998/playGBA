package com.kbkapps.playgba.cpu;

import com.kbkapps.playgba.cpu.constants.Instructions;
import com.kbkapps.playgba.cpu.opcodes.thumb.ArithmeticLogical;
import com.kbkapps.playgba.cpu.opcodes.thumb.OpCode;
import com.kbkapps.playgba.cpu.opcodes.thumb.SingleDataTransfer;

import static com.kbkapps.playgba.cpu.ArmV3Cpu.*;

public class ThumbCpu {

    private final Registers reg;
    private final GbaMemory gbaMem;

    public ThumbCpu(Registers registers, GbaMemory gbaMemory) {
        reg = registers;
        gbaMem = gbaMemory;
    }

    public void execute(OpCode opcode) throws UndefinedOpcodeException, WriteDeniedException {
        if (opcode == null) {
            System.out.println("tNOP");
            return;
        } else System.out.println("Executing: " + opcode);
        if (opcode.getInstruction() == Instructions.MOVS) {
            int regD = opcode.getRegDest();
            reg.setReg(regD, opcode.getImmediate());
            setFlags(reg.getReg(regD));
        } else if (opcode.getInstruction() == Instructions.LDR_PC) {
            int regD = opcode.getRegDest();
            int regDValue = gbaMem.read32(getPC() + opcode.getImmediate() * 4);
            reg.setReg(regD, regDValue);
        } else if (opcode.getInstruction() == Instructions.STR)
            storeWord((SingleDataTransfer) opcode);
        else if (opcode.getInstruction() == Instructions.ADD)
            add((ArithmeticLogical) opcode);
        else
            throw new UndefinedOpcodeException(opcode.toString());
    }

    private void add(ArithmeticLogical opcode) {
        int regD = opcode.getRegDest();
        int operand1 = reg.getReg(opcode.getRegSource());
//        System.out.println("op1 = " + Integer.toHexString(operand1));
        int operand2 = opcode.isImmediate() ? opcode.getImmediate() : reg.getReg(opcode.getRegNext());
//        System.out.println("op2 = " + Integer.toHexString(operand2));
        long result = operand1 + operand2;
        int regDValue = (int) result;
//        System.out.println("Result = 0x" + Long.toHexString(result) + " => 0x" + Integer.toHexString(regDValue));
        reg.setReg(regD, regDValue);
        setAllFlags(result);
    }

    private void storeWord(SingleDataTransfer opcode) throws WriteDeniedException {
        int regDValue = reg.getReg(opcode.getRegDest());
        int memPointer = reg.getReg(opcode.getBaseReg()) + reg.getReg(opcode.getOffsetReg());
//        System.out.println(Integer.toHexString(memPointer));
        gbaMem.write32(memPointer, regDValue);
    }

    private void setAllFlags(long result) {
        int flags = 0;
        if (((result >> 31) & 1) != 0)
            flags |= N;
        if (((int) result) == 0)
            flags |= Z;
        if ((result >> 32) > 0)
            flags |= C;
//   TODO if ((result >> 32) > 0)
//            flags |= V;
        reg.setFlags(flags);
    }

    private void setFlags(long result) {
        int flags = 0;
        if (((result >> 31) & 1) != 0)
            flags |= N;
        if (((int) result) == 0)
            flags |= Z;
        reg.setFlags(flags);
    }

    private int getPC() {
        return reg.getReg(PC) - 2;
    }
}
