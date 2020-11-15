package com.kbkapps.playgba.cpu;

import com.kbkapps.playgba.cpu.opcodes.OpCode;
import io.cucumber.java.en.And;
import io.cucumber.java.en.Given;
import io.cucumber.java.en.Then;
import io.cucumber.java.en.When;

import static org.assertj.core.api.Assertions.assertThat;

public class ArmInstructionStepDefinitions {
    Registers reg = new Registers();
    GbaMemory mem = new GbaMemory();
    ArmV3Cpu armCpu = new ArmV3Cpu(reg, mem);
    private OpCode opcode;


    @Given("^pc is ([0-9]+)$")
    public void pcIs(String pc) {
        reg.setReg(ArmV3Cpu.PC, Integer.parseUnsignedInt(pc));
    }

    @When("^i try to decode ([0-9a-f]{1,2}) ([0-9a-f]{1,2}) ([0-9a-f]{1,2}) ([0-9a-f]{1,2})$")
    public void decodeOpcode(String arg0, String arg1, String arg2, String arg3) throws UndefinedOpcodeException {
        int opcodeEncoded = getIntFromBytes(arg0, arg1, arg2, arg3);
//        System.out.println("opcodeEncoded = " + Integer.toUnsignedString(opcodeEncoded, 16));
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
        assertThat(reg.getPSR(Registers.CPSR)).inHexadecimal().isEqualTo(getIntFromBytes(arg3, arg2, arg1, arg0));
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

    @Given("^word 0x([0-9a-f]{1,8}) is present in memory 0x([0-9a-f]{1,8})$")
    public void isPresentInMemoryX(String data, String address) throws WriteDeniedException {
        mem.write32(Integer.parseUnsignedInt(address, 16), Integer.parseUnsignedInt(data, 16));
    }

    @Given("^byte 0x([0-9a-f]{1,2}) is present in memory 0x([0-9a-f]{1,8})$")
    public void bIsPresentInMemoryX(String data, String address) throws WriteDeniedException {
        mem.write8(Integer.parseUnsignedInt(address, 16), Byte.parseByte(data, 16));
    }

    @Given("^byte ([0-9]+) is present in memory 0x([0-9a-f]{1,8})$")
    public void bIsPresentInMemory(String data, String address) throws WriteDeniedException {
        mem.write8(Integer.parseUnsignedInt(address, 16), Byte.parseByte(data));
    }

    @Given("^R([0-9]{1,2}) is 0x([0-9a-f]{1,8})$")
    public void rIsX(String regNo, String data) {
        reg.setReg(Integer.parseInt(regNo), Integer.parseUnsignedInt(data, 16));
    }

    @Then("^([0-9a-f]{1,8}) should be present in memory 0x([0-9a-f]{1,8})$")
    public void shouldBePresentInMemory(String data, String address) {
        assertThat(mem.read8(Integer.parseUnsignedInt(address, 16))).inHexadecimal().isEqualTo(Byte.parseByte(data, 16));
    }

    @Given("^lr is ([0-9]+)$")
    public void lrIs(String pc) {
        reg.setReg(ArmV3Cpu.LR, Integer.parseUnsignedInt(pc));
    }

    @Then("^lr must be ([0-9]+)$")
    public void lrMustBe(String expectedPC) {
        assertThat(reg.getReg(ArmV3Cpu.LR)).isEqualTo(Integer.parseUnsignedInt(expectedPC));
    }

    @Then("mode must be {word}")
    public void modeMustBe(String expectedMode) {
        assertThat(reg.getMode()).isEqualTo(expectedMode);
    }

    @Then("Irq must be enabled")
    public void irqMustBeEnabled() {
        assertThat(reg.irqEnable).isTrue();
    }

    @Then("Irq must be disabled")
    public void irqMustBeDisabled() {
        assertThat(reg.irqEnable).isFalse();
    }

    @Then("CPU must run in ARM")
    public void cpuMustRunInARM() {
        assertThat(reg.thumbMode).isFalse();
    }

    @Then("CPU must run in THUMB")
    public void cpuMustRunInTHUMB() {
        assertThat(reg.thumbMode).isTrue();
    }

    @Then("^SP must be 0x([0-9a-f]{1,8})$")
    public void spMustBeX(String expectedSP) {
        assertThat(reg.getReg(ArmV3Cpu.SP)).inHexadecimal().isEqualTo(Integer.parseUnsignedInt(expectedSP, 16));
    }

    @Then("^SPSR must be ([0-9a-f]{1,2}) ([0-9a-f]{1,2}) ([0-9a-f]{1,2}) ([0-9a-f]{1,2})$")
    public void spsrMustBe(String arg0, String arg1, String arg2, String arg3) {
        assertThat(reg.getPSR(Registers.SPSR)).inHexadecimal().isEqualTo(getIntFromBytes(arg3, arg2, arg1, arg0));
    }

//    @Given("^SP is ([0-9a-f]{1,8})$")
//    public void spIs(String sp) {
//        reg.setReg(ArmV3Cpu.SP,Integer.parseUnsignedInt(sp,16));
//    }
}
