package com.kbkapps.playgba.cpu.constants;

public enum Instructions {
    B("branch"), CMP("compare"), MOV("move"), LDR("load"), TEQ("exclusive test"), MRS("move psr to reg"), ORR("logical or"), MSR("move reg to psr");

    private final String message;

    Instructions(String message) {
        this.message = message;
    }

    @Override
    public String toString() {
        return message;
    }
}
