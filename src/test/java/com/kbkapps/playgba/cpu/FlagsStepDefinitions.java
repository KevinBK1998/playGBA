package com.kbkapps.playgba.cpu;

import com.kbkapps.playgba.cpu.constants.Flags;
import com.kbkapps.playgba.cpu.constants.Instructions;
import com.kbkapps.playgba.cpu.opcodes.armv3.ArithmeticLogical;
import com.kbkapps.playgba.cpu.opcodes.armv3.Branch;
import com.kbkapps.playgba.cpu.opcodes.armv3.OpCode;
import io.cucumber.java.en.And;
import io.cucumber.java.en.Given;
import io.cucumber.java.en.Then;
import io.cucumber.java.en.When;

import static org.assertj.core.api.Assertions.assertThat;

public class FlagsStepDefinitions {
    Registers reg = new Registers();
    ArmV3Cpu armCpu = new ArmV3Cpu(reg);

    @Given("^PC is ([0-9]+)$")
    public void setPC(String data) {
        reg.setPC(Integer.parseUnsignedInt(data));
    }

    @When("I try to execute B {word} {int}")
    public void executeB(String condition, int label) throws UndefinedOpcodeException {
        Flags cond = Flags.valueOf(condition);
        armCpu.execute(new Branch(Instructions.B, cond, label));
    }

    @Then("^I should be at ([0-9]+)$")
    public void shouldBeAt(String expectedPC) {
        assertThat(reg.getPC()).isEqualTo(Integer.parseUnsignedInt(expectedPC));
    }

    @Given("^CPSR is 0x([0-9a-f]{1,8})$")
    public void cpsrIsX(String data) {
        reg.setPSR(Registers.CPSR, Integer.parseUnsignedInt(data, 16));
    }

    @When("^I try to execute compare r0 0x([0-9a-f]{1,3})$")
    public void iTryToExecuteCompareData(String data) throws UndefinedOpcodeException {
        OpCode opCode = new ArithmeticLogical(Instructions.CMP, Flags.AL, true, Integer.parseUnsignedInt(data, 16));
        armCpu.execute(opCode);
    }

    @Then("^CPSR must be 0x([0-9a-f]{1,8})$")
    public void cpsrMustBeX(String expectedData) {
        assertThat(reg.getPSR(Registers.CPSR)).inHexadecimal().isEqualTo(Integer.parseUnsignedInt(expectedData, 16));
    }

    @And("^r0 is 0x([0-9a-f]{1,8})$")
    public void rIsX(String data) {
        reg.setReg(0, Integer.parseUnsignedInt(data, 16));
    }
}
