package com.kbkapps.playgba.cpu;

import io.cucumber.java.en.Given;
import io.cucumber.java.en.Then;
import io.cucumber.java.en.When;

import static org.assertj.core.api.Assertions.assertThat;

public class RegistersStepDefinitions {
    private int data;
    private Registers registers;

    @Given("All registers are initialised")
    public void initialiseRegisters() {
        registers = new Registers();
    }

    @When("^I read from R([0-9]{1,2})$")
    public void readFromReg(int index) {
        data = registers.getReg(index);
    }

    @Then("^I should get ([0-9]+)$")
    public void shouldGetZero(String expected) {
        assertThat(data).isEqualTo(Integer.parseUnsignedInt(expected));
    }

    @When("^I write ([0-9]+) to R([0-9]{1,2})$")
    public void writeToReg(String data, int index) {
        registers.setReg(index, Integer.parseUnsignedInt(data));
    }

    @Then("^([0-9]+) should be present in R([0-9]{1,2})$")
    public void shouldBePresentInReg(String data, int index) {
        assertThat(registers.getReg(index)).isEqualTo(Integer.parseUnsignedInt(data));
    }

    @When("I switch mode to {word}")
    public void switchMode(String mode) {
        registers.setMode(mode);
    }

    @When("^I read from ([CS]{1})PSR$")
    public void readFromPSR(String type) {
        data = registers.getPSR(type.charAt(0));
    }

    @When("^I write ([0-9]+) to ([CS]{1})PSR$")
    public void writeToPSR(String data, String type) {
        registers.setPSR(type.charAt(0), Integer.parseUnsignedInt(data));
    }

    @Then("^([0-9]+) should be present in ([CS]{1})PSR$")
    public void shouldBePresentInPSR(String data, String type) {
        assertThat(registers.getPSR(type.charAt(0))).isEqualTo(Integer.parseUnsignedInt(data));
    }
}
