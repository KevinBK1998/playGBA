package com.kbkapps.playgba.cpu.constants;

public enum Flags {
    EQ("if equal"), NE("not equal"), CS("carry set"), CC("carry cleared"),
    MI("minus"), PL("plus"), VS("signed overflow"), VC("signed no overflow"),
    HI("unsigned higher"), LS("unsigned lower or same"), GE("signed greater or equal"),
    LT("signed less than"), GT("signed greater than"), LE("signed less or equal"),
    AL("always");
    private final String message;

    Flags(String message) {
        this.message = message;
    }

    @Override
    public String toString() {
        return message;
    }
}
