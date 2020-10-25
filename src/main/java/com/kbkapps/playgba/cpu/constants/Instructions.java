package com.kbkapps.playgba.cpu.constants;

public enum Instructions {
    B("branch"), CMP("compare"), MOV("move"), LDR("load");

    private final String message;

    Instructions(String message) {
        this.message = message;
    }

    @Override
    public String toString() {
        return message;
    }
}
