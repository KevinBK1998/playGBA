package com.kbkapps.playgba.cpu;

public enum Instructions {
    B("branch");
    private final String message;

    Instructions(String message) {
        this.message = message;
    }

    @Override
    public String toString() {
        return message;
    }
}
