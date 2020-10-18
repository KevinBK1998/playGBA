package com.kbkapps.playgba.cpu;

public enum Instructions {
    B;

    @Override
    public String toString() {
        switch (this) {
            case B:
                return "branch";
            default:
                return super.toString();
        }
    }
}
