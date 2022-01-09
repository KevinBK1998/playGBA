package com.kbkapps.playgba.cpu;

import com.kbkapps.playgba.cpu.opcodes.armv3.OpCode;

public class GbaCpu {
    public static final int N_STEPS = 600;
    public int opcodeEncoded;
    GbaMemory gbaMemory = new GbaMemory();
    Registers reg = new Registers();
    ArmV3Cpu armCpu = new ArmV3Cpu(reg, gbaMemory);
    ThumbCpu thumbCpu = new ThumbCpu(reg, gbaMemory);
    OpCode opcodeDecoded;
    com.kbkapps.playgba.cpu.opcodes.thumb.OpCode thumbOpcodeDecoded;
    private int prevPc;
    private int width = 4;

    public static void main(String[] args) {
        GbaCpu cpu = new GbaCpu();
        //TODO: BX in arm malfunctioning
        int i = 0;
        try {
            for (; i < N_STEPS; i++)
                cpu.trigger();
        } catch (UndefinedOpcodeException | WriteDeniedException e) {
            System.out.println(e.getMessage());
        }
        System.out.println(i + " instructions executed");
        System.out.println("Registers:");
        System.out.println(cpu.reg);
    }

    public void trigger() throws UndefinedOpcodeException, WriteDeniedException {
        if (reg.thumbMode)
            thumbTrigger();
        else
            armTrigger();
    }

    private void thumbTrigger() throws UndefinedOpcodeException, WriteDeniedException {
        thumbCpu.execute(thumbOpcodeDecoded);
//        System.out.println("reg = " + reg);
        if (!reg.thumbMode) {
            width = 4;
        }
        fetchInstruction();
        decodeInstruction();
        reg.step();
    }

    public void armTrigger() throws UndefinedOpcodeException {
        armCpu.execute(opcodeDecoded);
//        System.out.println("reg = " + reg);
        if (reg.thumbMode)
            width = 2;
        fetchInstruction();
        decodeInstruction();
        reg.step();
    }

    private void decodeInstruction() throws UndefinedOpcodeException {
        if (width > 2)
            opcodeDecoded = OpCode.decodeOpcode(opcodeEncoded);
        else
            thumbOpcodeDecoded = com.kbkapps.playgba.cpu.opcodes.thumb.OpCode.decodeOpcode((short) opcodeEncoded);
    }

    private void fetchInstruction() {
        int pc = reg.getPC();
        if (pc != prevPc || pc == 0) {
            System.out.println("\nFlushed Cache");
            opcodeDecoded = null;
            thumbOpcodeDecoded = null;
        }
        opcodeEncoded = 0;
        System.out.println("Fetching: PC=" + pc + " (0x" + Integer.toHexString(pc) + ")");
        for (int i = 0; i < width; i++) {
            byte byteValue = armCpu.gbaMemory.read8(pc + i);
            System.out.print(getUnsignedStringFromByte(byteValue) + " ");
            opcodeEncoded += Byte.toUnsignedInt(byteValue) << (8 * i);
        }
        System.out.println("was Fetched");
        prevPc = pc + width;
    }

    private static String getUnsignedStringFromByte(byte datum) {
        String result = Integer.toUnsignedString(datum, 16);
        if (result.length() == 8)
            return result.substring(6);
        if (result.length() == 1)
            return "0" + result;
        return result;
    }

    public void runTimes(int times) {
        try {
            for (int i = 0; i < times; i++)
                trigger();
        } catch (UndefinedOpcodeException | WriteDeniedException e) {
            System.out.println(e.getMessage());
        }
    }
}
