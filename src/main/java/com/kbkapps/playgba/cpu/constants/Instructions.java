package com.kbkapps.playgba.cpu.constants;

public enum Instructions {
    B("branch"),
    CMP("compare"),
    MOV("move"),
    MOVS("move-s"),
    LDR("load"),
    LDMIA("load from memory"),
    LDR_PC("load pc-relative"),
    TEQ("exclusive test"),
    MRS("move psr to reg"),
    ORR("logical or"),
    MSR("move reg to psr"),
    STR("store"),
    STRMIA("store in memory"),
    BL("branch with link"),
    ADD("add"),
    BX("exchanging branch"),
    PUSH("push"),
    POP("pop"),
    ADD_SP("add sp"),
    STR_SP("store sp-relative"),
    MVN("invert");

    private final String message;

    Instructions(String message) {
        this.message = message;
    }

    @Override
    public String toString() {
        return message;
    }
}
