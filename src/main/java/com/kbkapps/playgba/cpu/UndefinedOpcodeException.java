package com.kbkapps.playgba.cpu;

public class UndefinedOpcodeException extends Exception {
    public UndefinedOpcodeException(String msg) {
        super("Undefined Opcode: " + msg);
    }

    public UndefinedOpcodeException(int opcodeEncoded) {
        this(String.format("%08x", opcodeEncoded));
    }

    public UndefinedOpcodeException(short opcodeEncoded) {
        this(String.format("%04x", opcodeEncoded));
    }
}
