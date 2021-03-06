package com.kbkapps.playgba.cpu;

import com.kbkapps.playgba.cpu.opcodes.thumb.OpCode;
import io.cucumber.java.en.And;
import io.cucumber.java.en.Then;
import io.cucumber.java.en.When;

import static org.assertj.core.api.Assertions.assertThat;

public class ThumbInstructionsStepDefinitions {
    private OpCode opcode;
    Registers reg = new Registers();
    ThumbCpu thumbCpu = new ThumbCpu(reg, new GbaMemory());

    @When("^I try to decode ([0-9a-f]{1,2}) ([0-9a-f]{1,2})$")
    public void decodeOpcode(String arg0, String arg1) throws UndefinedOpcodeException {
        short opcodeEncoded = getIntFromBytes(arg0, arg1);
//        System.out.println("opcodeEncoded = " + Integer.toUnsignedString(opcodeEncoded, 16));
        opcode = OpCode.decodeOpcode(opcodeEncoded);
    }

    @Then("I should see {string}")
    public void iShouldSee(String message) {
        System.out.println("opcodeDecoded = " + opcode.toString());
        assertThat(opcode.toString()).isEqualTo(message);
    }

    @When("^I try to execute ([0-9a-f]{1,2}) ([0-9a-f]{1,2})$")
    public void iTryToExecute(String arg0, String arg1) throws UndefinedOpcodeException {
        thumbCpu.execute(OpCode.decodeOpcode(getIntFromBytes(arg0, arg1)));
        reg.step();
    }

    @Then("^r([0-9]{1,2}) must be ([0-9a-f]{1,8})$")
    public void rMustBe(String regNo, String expectedData) {
        assertThat(reg.getReg(Integer.parseInt(regNo))).isEqualTo(Integer.parseUnsignedInt(expectedData));
    }

    @And("^cpsr must be ([0-9a-f]{1,2}) ([0-9a-f]{1,2}) ([0-9a-f]{1,2}) ([0-9a-f]{1,2})$")
    public void cpsrMustBe(String arg0, String arg1, String arg2, String arg3) {
        assertThat(reg.getPSR(Registers.CPSR)).inHexadecimal().isEqualTo(getIntFromBytes(arg3, arg2, arg1, arg0));
    }

    private short getIntFromBytes(String arg0, String arg1) {
        return (short) (Integer.parseInt(arg0, 16) + (Integer.parseInt(arg1, 16) << 8));
    }

    private int getIntFromBytes(String arg0, String arg1, String arg2, String arg3) {
        return Integer.parseInt(arg0, 16) + (Integer.parseInt(arg1, 16) << 8) + (Integer.parseInt(arg2, 16) << 16) + (Integer.parseInt(arg3, 16) << 24);
    }
}
