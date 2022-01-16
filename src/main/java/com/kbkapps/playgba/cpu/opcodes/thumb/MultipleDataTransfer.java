package com.kbkapps.playgba.cpu.opcodes.thumb;

import com.kbkapps.playgba.cpu.UndefinedOpcodeException;
import com.kbkapps.playgba.cpu.constants.Instructions;

import java.util.ArrayList;
import java.util.List;
import java.util.stream.Collectors;

import static com.kbkapps.playgba.cpu.ArmV3Cpu.LR;
import static com.kbkapps.playgba.cpu.ArmV3Cpu.PC;
import static com.kbkapps.playgba.cpu.ArmV3Cpu.SP;

public class MultipleDataTransfer extends OpCode {
    private final List<Integer> rList;

    public MultipleDataTransfer(Instructions instructions, int regB, int rlist) {
        super(instructions, (byte) regB);
        rList = new ArrayList<>();
        for (int i = 0; i < 8; i++) {
            if (((rlist >> i) & 1) != 0)
                rList.add(i);
        }
        if (((rlist >> 8) & 1) != 0)
            rList.add(getMSB());
    }

    public static OpCode decodeOpcode(short opcodeEncoded) throws UndefinedOpcodeException {
        Instructions instructions;
        int regB = (opcodeEncoded >> 8) & 7;
        int rlist = opcodeEncoded & 0x1FF;
        switch (((opcodeEncoded >> 11) & 0x1f)) {
            case 0b10110:
                instructions = Instructions.PUSH;
                return new MultipleDataTransfer(instructions, SP, rlist);
            case 0b10111:
                instructions = Instructions.POP;
                return new MultipleDataTransfer(instructions, SP, rlist);
            case 0b11000:
                instructions = Instructions.STRMIA;
                return new MultipleDataTransfer(instructions, regB, rlist & 0xFF);
            case 0b11001:
                instructions = Instructions.LDMIA;
                return new MultipleDataTransfer(instructions, regB, rlist & 0xFF);
            default:
                throw new UndefinedOpcodeException(opcodeEncoded);
        }
    }

    public List<Integer> getListOfRegisters() {
        return rList;
    }

    private String printListOfRegisters() {
        return rList.stream()
                .map(this::getRegName)
                .collect(Collectors.joining(","));
    }

    private int getMSB() {
        return instruction == Instructions.PUSH ? LR : PC;
    }

    @Override
    public String toString() {
        return instruction.toString() + " " + getRegName(regDest) + "!, {" + printListOfRegisters() + "}";
    }
}
