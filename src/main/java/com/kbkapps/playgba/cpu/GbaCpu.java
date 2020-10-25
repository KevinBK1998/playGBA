package com.kbkapps.playgba.cpu;

import java.io.FileInputStream;
import java.io.IOException;

public class GbaCpu {
    public int opcodeEncoded;
    ArmV3Cpu armCpu = new ArmV3Cpu(new Registers());
    OpCode opcodeDecoded;
    byte[] gbaData;

    {
        try {
            FileInputStream in = new FileInputStream("src/main/resources/bios.bin");
            gbaData = new byte[in.available()];
            if (in.read(gbaData) < gbaData.length)
                System.out.println("ERROR: UNABLE TO READ");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void trigger() {
        executeOpcode();
        decodeInstruction();
        fetchInstruction();
        armCpu.step();
    }

    private void executeOpcode() {
        try {
            armCpu.execute(opcodeDecoded);
        } catch (UndefinedOpcodeException e) {
            System.out.println("Undefined Opcode: " + e.getMessage());
        }
    }

    private void decodeInstruction() {
        opcodeDecoded = OpCode.decodeOpcode(opcodeEncoded);
    }

    private void fetchInstruction() {
        int pc = armCpu.getPC();
        opcodeEncoded = 0;
        for (int i = pc; i < pc + 4; i++)
            opcodeEncoded += Byte.toUnsignedInt(gbaData[i]) << (8 * i);
    }

    public void setPC(int pc) {
        for (int i = 0; i < pc; i += 4)
            trigger();
    }

    public Registers getState() {
        return armCpu.getState();
    }
}
