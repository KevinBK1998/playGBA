package com.kbkapps.playgba.cpu;

import com.kbkapps.playgba.cpu.opcodes.OpCode;

import java.io.FileInputStream;
import java.io.IOException;

public class GbaCpu {
    public static final int N_STEPS = 8;
    public int opcodeEncoded;
    ArmV3Cpu armCpu = new ArmV3Cpu(new Registers());
    OpCode opcodeDecoded;
    byte[] gbaData;
    private int prevPc;

    {
        try {
            FileInputStream in = new FileInputStream("src/main/resources/bios.bin");
            gbaData = new byte[in.available()];
            if (in.read(gbaData) < gbaData.length)
                System.out.println("ERROR: UNABLE TO READ COMPLETELY");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static void main(String[] args) {
        GbaCpu cpu = new GbaCpu();
        try {
            for (int i = 0; i < N_STEPS; i++)
                cpu.trigger();
        } catch (UndefinedOpcodeException e) {
            System.out.println(e.getMessage());
        }
    }

    private void decodeInstruction() throws UndefinedOpcodeException {
        int pc = armCpu.getPC();
        if (pc != prevPc || pc == 0) {
            System.out.println("Flushed Cache");
            opcodeDecoded = null;
        } else
            opcodeDecoded = OpCode.decodeOpcode(opcodeEncoded);

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
        } catch (UndefinedOpcodeException e) {
            System.out.println(e.getMessage());
        }
    }

    public Registers getState() {
        return armCpu.getState();
    }

    public void trigger() throws UndefinedOpcodeException {
        armCpu.execute(opcodeDecoded);
        decodeInstruction();
        fetchInstruction();
        armCpu.step();
    }

    private void fetchInstruction() {
        int pc = armCpu.getPC();
        opcodeEncoded = 0;
        System.out.println("Fetching: PC=" + pc);
        for (int i = pc; i < pc + 4; i++) {
            byte byteValue = gbaData[i];
            System.out.print(getUnsignedStringFromByte(byteValue) + " ");
            this.opcodeEncoded += Byte.toUnsignedInt(byteValue) << (8 * i);
        }
        System.out.println("was Fetched");
        prevPc = pc + 4;
    }
}
