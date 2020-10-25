package com.kbkapps.playgba.cpu;

import io.cucumber.java.en.Given;
import io.cucumber.java.en.Then;
import io.cucumber.java.en.When;

import static org.assertj.core.api.Assertions.assertThat;

public class CpuStepDefinitions {

    private final GbaCpu gbaCpu = new GbaCpu();

    //    @When("cpu runs")
//    public void cpuRuns() throws IOException {
//        FileInputStream in = new FileInputStream("src/main/resources/bios.bin");
//        int pc = reg.getReg(ArmV3Cpu.PC);
//        byte[] data = new byte[in.available()];
//        if (in.read(data) > pc * 4) {
//            System.out.println("data[pc] = 0x" + getUnsignedStringFromByte(data[pc]));
//            System.out.println("data[pc+1] = 0x" + getUnsignedStringFromByte(data[pc + 1]));
//            System.out.println("data[pc+2] = 0x" + getUnsignedStringFromByte(data[pc + 2]));
//            System.out.println("data[pc+3] = 0x" + getUnsignedStringFromByte(data[pc + 3]));
//            byte[] opCode = {data[pc + 3], data[pc + 2], data[pc + 1], data[pc]};
//            armCpu.decode(opCode);
//        }
//    }
    @When("clock is triggered")
    public void clockIsTriggered() {
        gbaCpu.trigger();
    }

    @Then("current instruction is executed")
    public void currentInstructionIsExecuted() {
    }

    @Then("next instruction is decoded")
    public void nextInstructionIsDecoded() {
        assertThat(gbaCpu.opcodeDecoded).isEqualTo(OpCode.decodeOpcode(0xea_00_00_04));
    }

    @Then("the next instruction after that is fetched")
    public void theNextInstructionAfterThatIsFetched() {
        assertThat(gbaCpu.opcodeEncoded).inHexadecimal().isEqualTo(0xea_00_00_4c);
    }

    @Given("^PC is set to ([0-9]+)$")
    public void pcIs(String pc) {
        gbaCpu.setPC(Integer.parseUnsignedInt(pc, 16));
    }
}
