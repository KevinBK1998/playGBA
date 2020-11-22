package com.kbkapps.playgba.cpu;

import com.kbkapps.playgba.cpu.opcodes.OpCode;

public class GbaCpu {
    public static final int N_STEPS = 48;
    public int opcodeEncoded;
    GbaMemory gbaMemory = new GbaMemory();
    Registers reg = new Registers();
    ArmV3Cpu armCpu = new ArmV3Cpu(reg, gbaMemory);
    ThumbCpu thumbCpu = new ThumbCpu(reg, gbaMemory);
    OpCode opcodeDecoded;
    private int prevPc;
    private int width = 4;

    public static void main(String[] args) {
        GbaCpu cpu = new GbaCpu();
        int i = 0;
        try {
            for (; i < N_STEPS; i++)
                cpu.trigger();
        } catch (UndefinedOpcodeException e) {
            System.out.println(e.getMessage());
        }
        System.out.println(i + " instructions executed");
        System.out.println("Registers:");
        System.out.println(cpu.armCpu.getState());
    }

    public void trigger() throws UndefinedOpcodeException {
        if (reg.thumbMode)
            thumbTrigger();
        else
            armTrigger();
    }

    private void thumbTrigger() throws UndefinedOpcodeException {
        thumbCpu.execute(opcodeDecoded);
        if (!reg.thumbMode) {
            width = 4;
            return;
        }
        decodeInstruction();
        fetchInstruction();
        reg.step();
    }

    public void armTrigger() throws UndefinedOpcodeException {
        armCpu.execute(opcodeDecoded);
        if (reg.thumbMode) {
            width = 2;
            return;
        }
        decodeInstruction();
        fetchInstruction();
        reg.step();
    }

    private void decodeInstruction() throws UndefinedOpcodeException {
        int pc = armCpu.getPC();
        if (pc != prevPc || pc == 0) {
            System.out.println("Flushed Cache");
            opcodeDecoded = null;
        } else if (width > 2)
            opcodeDecoded = OpCode.decodeOpcode(opcodeEncoded);
        else
            opcodeDecoded = OpCode.decodeOpcode((short) opcodeEncoded);
    }

    private void fetchInstruction() {
        int pc = armCpu.getPC();
        opcodeEncoded = 0;
        System.out.println("Fetching: PC=" + pc);
        for (int i = pc; i < pc + width; i++) {
            byte byteValue = armCpu.gbaMemory.read8(i);
            System.out.print(getUnsignedStringFromByte(byteValue) + " ");
            this.opcodeEncoded += Byte.toUnsignedInt(byteValue) << (8 * i);
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

    public Registers getState() {
        return armCpu.getState();
    }

    public void runTimes(int times) {
        try {
            for (int i = 0; i < times; i++)
                trigger();
        } catch (UndefinedOpcodeException e) {
            System.out.println(e.getMessage());
        }
    }
}
