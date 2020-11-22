package com.kbkapps.playgba.cpu;

import com.kbkapps.playgba.cpu.opcodes.OpCode;
import io.cucumber.java.en.Then;
import io.cucumber.java.en.When;

import static org.assertj.core.api.Assertions.assertThat;

public class ThumbInstructionsStepDefinitions {
    private OpCode opcode;

    @When("^I try to decode ([0-9a-f]{1,2}) ([0-9a-f]{1,2})$")
    public void decodeOpcode(String arg0, String arg1) throws UndefinedOpcodeException {
        short opcodeEncoded = getIntFromBytes(arg0, arg1);
//        System.out.println("opcodeEncoded = " + Integer.toUnsignedString(opcodeEncoded, 16));
        opcode = OpCode.decodeOpcode(opcodeEncoded);
    }

    private short getIntFromBytes(String arg0, String arg1) {
        return (short) (Integer.parseInt(arg0, 16) + (Integer.parseInt(arg1, 16) << 8));
    }

    @Then("I should see {string}")
    public void iShouldSee(String message) {
        System.out.println("opcodeDecoded = " + opcode.toString());
        assertThat(opcode.toString()).isEqualTo(message);
    }
}
