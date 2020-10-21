package com.kbkapps.playgba.cpu;

import io.cucumber.java.en.Given;
import io.cucumber.java.en.Then;
import io.cucumber.java.en.When;

import java.io.FileInputStream;
import java.io.IOException;

import static org.assertj.core.api.Assertions.assertThat;

public class CpuStepDefinitions {
    Registers reg = new Registers();
    ArmV3Cpu armCpu = new ArmV3Cpu(reg);
    private OpCode opcode;

    @Given("CPSR is {word}")
    public void setCPSR(String state) {
//        reg.setPSR(Registers.CPSR, state.equals("high") ? 0xFF_FF_FF_FF : 0);
    }

    private static String getUnsignedStringFromByte(byte datum) {
        String result = Integer.toUnsignedString(datum, 16);
        if (result.length() == 8)
            return result.substring(6);
        return result;
    }

    @When("^i try to execute ([0-9a-f]{1,2}) ([0-9a-f]{1,2}) ([0-9a-f]{1,2}) ([0-9a-f]{1,2})$")
    public void executeOpcode(String arg0, String arg1, String arg2, String arg3) {
        int opcodeEncoded = Integer.parseInt(arg0, 16) + (Integer.parseInt(arg1, 16) << 8) + (Integer.parseInt(arg2, 16) << 16) + (Integer.parseInt(arg3, 16) << 24);
        opcode = armCpu.decode(opcodeEncoded);
        System.out.println("opcodeEncoded = " + Integer.toUnsignedString(opcodeEncoded, 16));
        armCpu.execute(opcode);
    }

    @Then("i should see {string}")
    public void shouldSeeMessage(String message) {
        System.out.println("opcode.toString() = " + opcode.toString());
        assertThat(opcode.toString()).isEqualTo(message);
    }

    @When("cpu runs")
    public void cpuRuns() throws IOException {
        FileInputStream in = new FileInputStream("src/main/resources/bios.bin");
        int pc = reg.getReg(ArmV3Cpu.PC);
        byte[] data = new byte[in.available()];
        if (in.read(data) > pc * 4) {
            System.out.println("data[pc] = 0x" + getUnsignedStringFromByte(data[pc]));
            System.out.println("data[pc+1] = 0x" + getUnsignedStringFromByte(data[pc + 1]));
            System.out.println("data[pc+2] = 0x" + getUnsignedStringFromByte(data[pc + 2]));
            System.out.println("data[pc+3] = 0x" + getUnsignedStringFromByte(data[pc + 3]));
            byte[] opCode = {data[pc + 3], data[pc + 2], data[pc + 1], data[pc]};
            armCpu.decode(opCode);
        }
    }

    @Then("^pc must be ([0-9]+)$")
    public void pcMustBe(String expectedPC) {
        assertThat(reg.getReg(ArmV3Cpu.PC)).isEqualTo(Integer.parseUnsignedInt(expectedPC));
    }

    @Given("^pc is ([0-9]+)$")
    public void pcIs(String pc) {
        reg.setReg(ArmV3Cpu.PC, Integer.parseUnsignedInt(pc));
    }

    @Then("^CPSR must be ([0-9a-f]{1,2}) ([0-9a-f]{1,2}) ([0-9a-f]{1,2}) ([0-9a-f]{1,2})$")
    public void cpsrMustBe(String arg0, String arg1, String arg2, String arg3) {
        int expectedCPSR = Integer.parseInt(arg0, 16) + (Integer.parseInt(arg1, 16) << 8) + (Integer.parseInt(arg2, 16) << 16) + (Integer.parseInt(arg3, 16) << 24);
        assertThat(reg.getPSR(Registers.CPSR)).isEqualTo(expectedCPSR);
    }
}
