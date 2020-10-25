package com.kbkapps.playgba.cpu;

import java.io.FileInputStream;
import java.io.IOException;

public class GbaCpu {
    public int opcodeEncoded;
    ArmV3Cpu armCpu = new ArmV3Cpu(new Registers());
    OpCode opcodeDecoded;

    public void trigger() {
//        executeOpcode();
        decodeInstruction();
        fetchInstruction();
    }

    private void decodeInstruction() {
        if (armCpu.getPC() > 4)
            opcodeDecoded = OpCode.decodeOpcode(opcodeEncoded);
    }

    private void fetchInstruction() {
        try {
            FileInputStream in = new FileInputStream("src/main/resources/bios.bin");
            int pc = armCpu.getPC();
            byte[] data = new byte[4];
            if (in.read(data) == 4) {
                opcodeEncoded = 0;
                for (int i = 0; i < 4; i++)
                    opcodeEncoded += Byte.toUnsignedInt(data[i]) << (8 * i);
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void setPC(int pc) {
        armCpu.setPc(pc);
    }
}
