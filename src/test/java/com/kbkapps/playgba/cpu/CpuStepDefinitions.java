package com.kbkapps.playgba.cpu;

import io.cucumber.java.en.And;
import io.cucumber.java.en.Given;
import io.cucumber.java.en.Then;
import io.cucumber.java.en.When;

import static org.assertj.core.api.Assertions.assertThat;

public class CpuStepDefinitions {
    Registers reg = new Registers();
    ArmV3Cpu armCpu = new ArmV3Cpu(reg);

    @Given("^PC is ([0-9]+)$")
    public void setPC(String data) {
        reg.setReg(ArmV3Cpu.PC, Integer.parseUnsignedInt(data));
    }

    @When("I try to execute B {word} {int}")
    public void executeB(String condition, int label) {
        armCpu.runOpcode("B", condition, label);
    }

    @Then("^I should be at ([0-9]+)$")
    public void shouldBeAt(String expectedPC) {
        assertThat(reg.getReg(ArmV3Cpu.PC)).isEqualTo(Integer.parseUnsignedInt(expectedPC));
    }

    @And("CPSR is {word}")
    public void setCPSR(String state) {
        reg.setPSR(Registers.CPSR, state.equals("high") ? 0xFF_FF_FF_FF : 0);
    }
}
