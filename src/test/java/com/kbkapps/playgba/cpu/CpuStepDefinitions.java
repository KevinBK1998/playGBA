package com.kbkapps.playgba.cpu;

import io.cucumber.java.en.Given;
import io.cucumber.java.en.Then;
import io.cucumber.java.en.When;

import static org.assertj.core.api.Assertions.assertThat;

public class CpuStepDefinitions {
    Registers reg = new Registers();
    ArmV3Cpu armCpu = new ArmV3Cpu(reg);

    @When("I try to execute B {word} {int}")
    public void executeB(String condition, int label) {
        armCpu.runOpcode("B", condition, label);
    }

    @Then("^I should be at ([0-9]+)$")
    public void shouldBeAt(String expectedPC) {
        assertThat(reg.getReg(ArmV3Cpu.PC)).isEqualTo(Integer.parseUnsignedInt(expectedPC));
    }

    @Given("^PC is ([0-9]+)$")
    public void setPC(String data) {
        reg.setReg(ArmV3Cpu.PC, Integer.parseUnsignedInt(data));
    }
}
