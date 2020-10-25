package com.kbkapps.playgba.cpu;

import io.cucumber.java.en.And;
import io.cucumber.java.en.Given;
import io.cucumber.java.en.Then;
import io.cucumber.java.en.When;

import static org.assertj.core.api.Assertions.assertThat;

public class InstructionStepDefinitions {
    Registers reg = new Registers();
    ArmV3Cpu armCpu = new ArmV3Cpu(reg);
    private OpCode opcode;

    @Given("^pc is ([0-9]+)$")
    public void pcIs(String pc) {
        reg.setReg(ArmV3Cpu.PC, Integer.parseUnsignedInt(pc));
    }

    @When("^i try to decode ([0-9a-f]{1,2}) ([0-9a-f]{1,2}) ([0-9a-f]{1,2}) ([0-9a-f]{1,2})$")
    public void decodeOpcode(String arg0, String arg1, String arg2, String arg3) throws UndefinedOpcodeException {
        int opcodeEncoded = getIntFromBytes(arg0, arg1, arg2, arg3);
        System.out.println("opcodeEncoded = " + Integer.toUnsignedString(opcodeEncoded, 16));
        opcode = OpCode.decodeOpcode(opcodeEncoded);
    }

    @Then("i should see {string}")
    public void shouldSeeMessage(String message) {
        System.out.println("opcodeDecoded = " + opcode.toString());
        assertThat(opcode.toString()).isEqualTo(message);
    }

    @Given("^CPSR is ([0-9a-f]{1,2}) ([0-9a-f]{1,2}) ([0-9a-f]{1,2}) ([0-9a-f]{1,2})$")
    public void cpsrIs(String arg0, String arg1, String arg2, String arg3) {
        reg.setPSR(Registers.CPSR, getIntFromBytes(arg3, arg2, arg1, arg0));
    }

    @When("^i try to execute ([0-9a-f]{1,2}) ([0-9a-f]{1,2}) ([0-9a-f]{1,2}) ([0-9a-f]{1,2})$")
    public void executeOpcode(String arg0, String arg1, String arg2, String arg3) throws UndefinedOpcodeException {
        armCpu.execute(OpCode.decodeOpcode(getIntFromBytes(arg0, arg1, arg2, arg3)));
        reg.step();
    }

    @Then("^pc must be ([0-9]+)$")
    public void pcMustBe(String expectedPC) {
        assertThat(reg.getReg(ArmV3Cpu.PC)).isEqualTo(Integer.parseUnsignedInt(expectedPC));
    }

    @Then("^CPSR must be ([0-9a-f]{1,2}) ([0-9a-f]{1,2}) ([0-9a-f]{1,2}) ([0-9a-f]{1,2})$")
    public void cpsrMustBe(String arg0, String arg1, String arg2, String arg3) {
        assertThat(reg.getPSR(Registers.CPSR)).isEqualTo(getIntFromBytes(arg3, arg2, arg1, arg0));
    }

    @And("^R([0-9]{1,2}) must be ([0-9]+)$")
    public void rMustBe(String regNo, String expectedData) {
        assertThat(reg.getReg(Integer.parseInt(regNo))).isEqualTo(Integer.parseUnsignedInt(expectedData));
    }

    private int getIntFromBytes(String arg0, String arg1, String arg2, String arg3) {
        return Integer.parseInt(arg0, 16) + (Integer.parseInt(arg1, 16) << 8) + (Integer.parseInt(arg2, 16) << 16) + (Integer.parseInt(arg3, 16) << 24);
    }

    @And("^R([0-9]{1,2}) must be 0x([0-9a-f]{1,8})$")
    public void rMustBeX(String regNo, String expectedData) {
        assertThat(reg.getReg(Integer.parseInt(regNo))).isEqualTo(Integer.parseUnsignedInt(expectedData, 16));
    }
}
