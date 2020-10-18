package com.kbkapps.playgba.cpu;

import io.cucumber.java.en.Given;
import io.cucumber.java.en.Then;
import io.cucumber.java.en.When;

import static org.assertj.core.api.Assertions.assertThat;

public class CpuStepDefinitions {
    public static final int PC = 15;
    Registers reg = new Registers();
    Instructions instructions = new Instructions(reg);

    @When("I try to execute B {word} {int}")
    public void executeB(String condition, int label) {
        instructions.setOpcode("B", label);
    }

    @Then("^I should be at ([0-9]+)$")
    public void shouldBeAt(String expectedPC) {
        assertThat(reg.getReg(PC)).isEqualTo(Integer.parseUnsignedInt(expectedPC));
    }

    @Given("^PC is ([0-9]+)$")
    public void setPC(String data) {
        reg.setReg(PC, Integer.parseUnsignedInt(data));
    }
}
