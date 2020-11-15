package com.kbkapps.playgba.cpu;

public class WriteDeniedException extends Exception {
    WriteDeniedException(int address) {
        super("Write is Denied to 0x" + Integer.toHexString(address));
    }
}
