package com.kbkapps.playgba.cpu;

public class UndefinedOpcodeException extends Exception {
    public UndefinedOpcodeException(String msg) {
        super(msg);
    }
}
