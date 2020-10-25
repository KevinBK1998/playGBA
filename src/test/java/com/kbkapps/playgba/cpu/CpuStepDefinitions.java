package com.kbkapps.playgba.cpu;

import io.cucumber.java.en.Given;
import io.cucumber.java.en.Then;
import io.cucumber.java.en.When;

import java.io.FileInputStream;
import java.io.IOException;

import static org.assertj.core.api.Assertions.assertThat;

public class CpuStepDefinitions {
    private final GbaCpu gbaCpu = new GbaCpu();

    @When("clock is triggered")
    public void clockIsTriggered() {
        gbaCpu.trigger();
    }

    @Then("current instruction is executed")
    public void currentInstructionIsExecuted() {
        int[] unbankedReg = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};//13
        int[] bankedReg = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};//10
        int[] savedStatusReg = {0, 0, 0, 0, 0};//5
        assertThat(gbaCpu.getState()).isEqualTo(new Registers(unbankedReg, bankedReg, savedStatusReg, 0, gbaCpu.armCpu.getPC(), 0));
    }

    @Then("next instruction is decoded")
    public void nextInstructionIsDecoded() {
        assertThat(gbaCpu.opcodeDecoded).isEqualTo(OpCode.decodeOpcode(0xea_00_00_04));
    }

    @Then("the next instruction after that is fetched")
    public void theNextInstructionAfterThatIsFetched() {
        try {
            FileInputStream in = new FileInputStream("src/main/resources/bios.bin");
            int pc = gbaCpu.armCpu.getPC();
            System.out.println("pc = " + pc);
            byte[] data = new byte[4];
            in.skip(pc - 4);
            if (in.read(data) == 4) {
                int opcodeEncoded = 0;
                for (int i = 0; i < 4; i++)
                    opcodeEncoded += Byte.toUnsignedInt(data[i]) << (8 * i);
                assertThat(gbaCpu.opcodeEncoded).inHexadecimal().isEqualTo(opcodeEncoded);
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @Given("^PC is set to ([0-9]+)$")
    public void pcIs(String pc) {
        gbaCpu.setPC(Integer.parseUnsignedInt(pc, 16));
    }
}
