package com.kbkapps.playgba.cpu;

import io.cucumber.java.en.Given;

public class CpuStepDefinitions {

    @Given("CPSR is {word}")
    public void setCPSR(String state) {
//        reg.setPSR(Registers.CPSR, state.equals("high") ? 0xFF_FF_FF_FF : 0);
    }
}
