package com.kbkapps.playgba.cpu;

import com.kbkapps.playgba.cpu.opcodes.armv3.OpCode;
import io.cucumber.java.en.Given;
import io.cucumber.java.en.Then;
import io.cucumber.java.en.When;

import java.io.FileInputStream;
import java.io.IOException;

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
        assertThat(gbaCpu.getState()).isEqualTo(new Registers(unbankedReg, bankedReg, savedStatusReg, gbaCpu.armCpu.getPC(), 0));
    }

    @Then("next instruction is decoded")
    public void nextInstructionIsDecoded() throws UndefinedOpcodeException {
        assertThat(gbaCpu.opcodeDecoded).isEqualTo(OpCode.decodeOpcode(0xe3_5e_00_00));
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

    @Given("^CPU runs ([0-9]+) times$")
    public void pcIs(String times) {
        gbaCpu.runTimes(Integer.parseUnsignedInt(times, 16));
    }
}
