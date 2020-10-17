package com.kbkapps.playgba.cpu;

import io.cucumber.java.en.Given;
import io.cucumber.java.en.Then;
import io.cucumber.java.en.When;

import static org.assertj.core.api.Assertions.assertThat;

public class CpuStepDefinitions {
    private Integer data;
    private Registers registers;

    @Given("All registers are initialised to zero")
    public void initialiseRegisters() {
        registers = new Registers();
    }

    @When("^I read from R([0-9]{1,2})$")
    public void readFromReg(int index) {
        data = registers.getReg(index);
    }

    @Then("I should get 0")
    public void shouldGetZero() {
        assertThat(data).isEqualTo(0);
    }

    @When("^I write ([0-9]+) to R([0-9]{1,2})$")
    public void writeToReg(String data, int index) {
        registers.setReg(Integer.parseUnsignedInt(data), index);
    }

    @Then("^([0-9]+) should be present in R([0-9]{1,2})$")
    public void shouldBePresentInReg(String data, int index) {
        assertThat(registers.getReg(index)).isEqualTo(Integer.parseUnsignedInt(data));
    }
}
