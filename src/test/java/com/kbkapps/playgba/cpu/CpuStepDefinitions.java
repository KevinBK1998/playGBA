package com.kbkapps.playgba.cpu;

import com.kbkapps.playgba.cpu.opcodes.armv3.OpCode;
import io.cucumber.java.en.Given;
import io.cucumber.java.en.Then;
import io.cucumber.java.en.When;

import static org.assertj.core.api.Assertions.assertThat;

public class CpuStepDefinitions {
    private final GbaCpu gbaCpu = new GbaCpu();

    @When("clock is triggered")
    public void clockIsTriggered() throws UndefinedOpcodeException {
        gbaCpu.armTrigger();
    }

    @Then("current instruction is executed")
    public void currentInstructionIsExecuted() {
        int[] unbankedReg = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};//13
        int[] bankedReg = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};//10
        int[] savedStatusReg = {0, 0, 0, 0, 0};//5
        assertThat(gbaCpu.reg).isEqualTo(new Registers(unbankedReg, bankedReg, savedStatusReg, gbaCpu.reg.getReg(15), 0));
    }

    @Then("next instruction is fetched")
    public void nextInstructionIsFetched() {
        assertThat(gbaCpu.opcodeEncoded).inHexadecimal().isEqualTo(0xe3_5e_00_00);
    }

    @Then("next instruction is decoded")
    public void nextInstructionIsDecoded() throws UndefinedOpcodeException {
        assertThat(gbaCpu.opcodeDecoded).isEqualTo(OpCode.decodeOpcode(0xe3_5e_00_00));
    }

    @Given("^CPU runs ([0-9]+) times$")
    public void pcIs(String times) {
        gbaCpu.runTimes(Integer.parseUnsignedInt(times, 16));
    }
}
