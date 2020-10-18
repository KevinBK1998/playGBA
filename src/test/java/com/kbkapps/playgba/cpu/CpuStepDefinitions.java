package com.kbkapps.playgba.cpu;

import io.cucumber.java.en.And;
import io.cucumber.java.en.Then;
import io.cucumber.java.en.When;

public class CpuStepDefinitions {
    Registers reg = new Registers();

    @When("I try to execute B {word} {int}")
    public void executeB(String condition, int label) {
    }

    @Then("I should be at {int}")
    public void shouldBeAt(int expectedPC) {
    }

    @And("^PC is ([0-9]+)$")
    public void setPC(String data) {
//        reg.setReg();
    }
}
