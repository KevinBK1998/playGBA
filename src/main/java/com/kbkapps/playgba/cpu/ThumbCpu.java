package com.kbkapps.playgba.cpu;

import com.kbkapps.playgba.cpu.constants.Instructions;
import com.kbkapps.playgba.cpu.opcodes.thumb.ArithmeticLogical;
import com.kbkapps.playgba.cpu.opcodes.thumb.Branch;
import com.kbkapps.playgba.cpu.opcodes.thumb.ExchangingBranch;
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
            int regDValue = gbaMem.read32(reg.getPC() - 2 + opcode.getImmediate() * 4);
            reg.setReg(regD, regDValue);
        } else if (opcode.getInstruction() == Instructions.STR) storeWord((SingleDataTransfer) opcode);
        else if (opcode.getInstruction() == Instructions.ADD) add((ArithmeticLogical) opcode);
        else if (opcode.getInstruction() == Instructions.B) branch((Branch) opcode);
        else if (opcode.getInstruction() == Instructions.BX) branch((ExchangingBranch) opcode);
        else throw new UndefinedOpcodeException(opcode.toString());
    }

    private void branch(ExchangingBranch opcode) {
        int regS = opcode.getRegSource();
//        System.out.println("regS = " + regS);
        int regSValue = reg.getReg(regS);
        System.out.printf("regSValue = 0x%x\n", regSValue);
        boolean armMode = (regSValue & 1) == 0;
        if (armMode) {
            regSValue = regSValue & 0xFF_FF_FF_FC;
//            System.out.printf("reg.getPSR(Registers.CPSR) = 0x%08x\n", reg.getPSR(Registers.CPSR));
            System.out.printf("newCPSR = 0x%08x\n", reg.getPSR(Registers.CPSR) & 0xFF_FF_FF_DF);
            reg.setPSR(Registers.CPSR, reg.getPSR(Registers.CPSR) & 0xFF_FF_FF_DF);
        }
        System.out.printf("PC = 0x%x\n", regSValue);
        reg.setReg(PC, regSValue - 4);
        System.out.printf("reg.getPC() = 0x%x\n", reg.getPC());
    }

    private void branch(Branch opcode) {
//        System.out.println("opcode.getCondition() = " + opcode.getCondition());
//        System.out.println("reg.canExecute(Flags.MI) = " + reg.canExecute(Flags.MI));
//        System.out.println("reg.canExecute(Flags.VS) = " + reg.canExecute(Flags.VS));
        if (reg.canExecute(opcode.getCondition())) {
            int newPC = reg.getReg(PC) + opcode.getOffset();
//            System.out.println("PC = " + newPC);
            reg.setReg(PC, newPC);
        }
    }

    private void add(ArithmeticLogical opcode) {
        int regD = opcode.getRegDest();
        System.out.println("reg.getReg(regD) = " + reg.getReg(regD));
        int operand1 = reg.getReg(opcode.getRegSource());
//        System.out.println("op1 = " + Integer.toHexString(operand1));
        int operand2 = opcode.isImmediate() ? opcode.getImmediate() : reg.getReg(opcode.getRegNext());
//        System.out.println("op2 = " + Integer.toHexString(operand2));
        long result = operand1 + operand2;
        int regDValue = (int) result;
//        System.out.println("Result = 0x" + Long.toHexString(result) + " => 0x" + Integer.toHexString(regDValue));
        reg.setReg(regD, regDValue);
//        System.out.println("reg.getReg(1) = " + reg.getReg(1));
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
        if (((result >> 31) & 1) != 0) flags |= N;
        if (((int) result) == 0) flags |= Z;
        if ((result >> 32) > 0) flags |= C;
//   TODO if ((result >> 32) > 0)
//            flags |= V;
        reg.setFlags(flags);
    }

    private void setFlags(long result) {
        int flags = 0;
        if (((result >> 31) & 1) != 0) flags |= N;
        if (((int) result) == 0) flags |= Z;
        reg.setFlags(flags);
    }
}
