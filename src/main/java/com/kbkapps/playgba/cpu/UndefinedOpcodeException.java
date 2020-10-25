package com.kbkapps.playgba.cpu;

public class UndefinedOpcodeException extends Exception {
    public UndefinedOpcodeException(String msg) {
        super("Undefined Opcode: " + msg);
    }

    public UndefinedOpcodeException(int opcodeEncoded) {
        this(Integer.toUnsignedString(opcodeEncoded, 16));
    }
}
