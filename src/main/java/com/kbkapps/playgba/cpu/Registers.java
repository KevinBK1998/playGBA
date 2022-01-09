package com.kbkapps.playgba.cpu;

import com.kbkapps.playgba.cpu.constants.Flags;

import static com.kbkapps.playgba.cpu.constants.Flags.*;

public class Registers {
    public static final int SYSTEM_OR_USER = 0;
    public static final int INTERRUPT_REQUEST = 1;
    public static final int SUPERVISOR = 2;
    public static final int ABORT = 3;
    public static final int UNDEFINED = 4;
    static final char CPSR = 'C';
    static final char SPSR = 'S';
    //Registers 0-12
    private final int[] unbankedReg = new int[13];
    //Registers 13(SP) and 14(LR)
    private final int[] bankedReg = new int[10];
    private final int[] savedStatusReg = new int[5];
    public boolean irqEnable;
    public boolean thumbMode;
    private int mode;
    private boolean privileged;
    //PC
    private int reg15 = 4;
    private int currentStatusReg;

    public Registers() {
        System.out.println("Start State: " + this);
    }

    public Registers(int[] unbankedReg, int[] bankedReg, int[] savedStatusReg, int r15, int cpsr) {
        System.arraycopy(unbankedReg, 0, this.unbankedReg, 0, 13);
        System.arraycopy(bankedReg, 0, this.bankedReg, 0, 10);
        System.arraycopy(savedStatusReg, 0, this.savedStatusReg, 0, 5);
        reg15 = r15;
        currentStatusReg = cpsr;
        setControlBits();
    }

    private void setControlBits() {
        irqEnable = (currentStatusReg & 0x80) == 0;
        thumbMode = (currentStatusReg & 0x20) != 0;
        int modeMeta = currentStatusReg & 0x1F;
        switch (modeMeta) {
            case 0:
            case 16:
                mode = SYSTEM_OR_USER;
                privileged = false;
                break;
            case 2:
            case 18:
                mode = INTERRUPT_REQUEST;
                break;
            case 3:
            case 19:
                mode = SUPERVISOR;
                break;
            case 23:
                mode = ABORT;
                break;
            case 27:
                mode = UNDEFINED;
                break;
            case 31:
                mode = SYSTEM_OR_USER;
                privileged = true;
                break;
            default:
                throw new IndexOutOfBoundsException("FIQ:" + modeMeta);
        }
    }

    int getReg(int index) {
        if (index < 0)
            throw new IndexOutOfBoundsException();
        if (index < 13)
            return unbankedReg[index];
        if (index < 15)
            return bankedReg[getBankedIndex(index)];
        if (index == 15)
            return reg15;
        throw new IndexOutOfBoundsException();
    }

    int getPC() {
        return reg15 - getStepAmount();
    }

    private int getBankedIndex(int index) {
        index -= 13;
        return index * 5 + mode;
    }

    void setReg(int index, int data) {
        if (index < 0)
            throw new IndexOutOfBoundsException();
        if (index < 13)
            unbankedReg[index] = data;
        else if (index < 15)
            bankedReg[getBankedIndex(index)] = data;
        else if (index == 15) {
            assert data > 0;
            reg15 = data;
        } else
            throw new IndexOutOfBoundsException();
    }

    void setPC(int data) {
        int newPC = data + getStepAmount();
        assert newPC > 0;
        reg15 = newPC;
    }

    public int getPSR(char type) {
        if (type == CPSR)
            return currentStatusReg;
        if (type == SPSR)
            return savedStatusReg[mode];
        throw new IndexOutOfBoundsException();
    }

    public void setPSR(char type, int data) {
        if (type == CPSR) {
            currentStatusReg = data;
            setControlBits();
        } else if (type == SPSR)
            savedStatusReg[mode] = data;
        else throw new IndexOutOfBoundsException();
    }

    public void setFlags(int flags) {
        currentStatusReg = currentStatusReg & 0xF_FF_FF_FF;
        currentStatusReg = currentStatusReg | flags;
    }

    public boolean canExecute(Flags cond) {
        //NegativeZeroCarryOverflow(V)----  --------  --------  IrqFiqThumbMMMMMode
        if (cond == AL) return true;
        else if (cond == MI) return currentStatusReg < 0;
        else if (cond == PL) return currentStatusReg >= 0;
        else if (cond == EQ) return (currentStatusReg & 0x40_00_00_00) != 0;
        else if (cond == NE) return (currentStatusReg & 0x40_00_00_00) == 0;
        else if (cond == CS) return (currentStatusReg & 0x20_00_00_00) != 0;
        else if (cond == CC) return (currentStatusReg & 0x20_00_00_00) == 0;
        else if (cond == VS) return (currentStatusReg & 0x10_00_00_00) != 0;
        else if (cond == VC) return (currentStatusReg & 0x10_00_00_00) == 0;
        else if (cond == HI) return canExecute(CS) && canExecute(NE);
        else if (cond == LS) return canExecute(CC) || canExecute(EQ);
        else if (cond == GE) return canExecute(MI) == canExecute(VS);
        else if (cond == LT) return canExecute(MI) != canExecute(VS);
        else if (cond == GT) return canExecute(NE) && canExecute(GE);
        else if (cond == LE) return canExecute(EQ) || canExecute(LT);
        else throw new IndexOutOfBoundsException(cond.name());
    }

    public void step() {
        reg15 += getStepAmount();
    }

    private int getStepAmount() {
        return thumbMode ? 2 : 4;
    }

    @Override
    public boolean equals(Object obj) {
        if (obj instanceof Registers)
            return this.toString().equalsIgnoreCase(obj.toString());
        return false;
    }

    @Override
    public String toString() {
        StringBuilder regState = new StringBuilder();
        for (int i = 0; i < 13; i++)
            regState.append("r").append(i).append(":0x").append(Integer.toUnsignedString(unbankedReg[i], 16)).append(" ");
        regState.append("r13:0x").append(Integer.toUnsignedString(getReg(13), 16)).append(" ");
        regState.append("r14:0x").append(Integer.toUnsignedString(getReg(14), 16)).append(" ");
        regState.append("r15:0x").append(Integer.toUnsignedString(reg15, 16)).append(" ");
        regState.append("\nsp: ");
        for (int i = 0; i < 5; i++)
            regState.append("0x").append(Integer.toUnsignedString(bankedReg[i], 16)).append(" ");
        regState.append("\nlr: ");
        for (int i = 5; i < 10; i++)
            regState.append("0x").append(Integer.toUnsignedString(bankedReg[i], 16)).append(" ");
        regState.append("\nspsr: ");
        for (int i = 0; i < 5; i++)
            regState.append("0x").append(Integer.toUnsignedString(savedStatusReg[i], 16)).append(" ");
        regState.append("\nmode:").append(Integer.toUnsignedString(mode, 16)).append(" ")
                .append("privileged:").append(privileged).append(" ")
                .append("pc:0x").append(Integer.toUnsignedString(getPC(), 16)).append(" ")
                .append("cpsr:0x").append(Integer.toUnsignedString(currentStatusReg, 16));

        return regState.toString();
    }

    public String getMode() {
        switch (mode) {
            case SYSTEM_OR_USER:
                if (privileged)
                    return "sys";
                else
                    return "usr";
            case INTERRUPT_REQUEST:
                return "irq";
            case SUPERVISOR:
                return "svc";
            case ABORT:
                return "abt";
            case UNDEFINED:
                return "und";
            default:
                return null;
        }
    }

    public void setMode(String mode) {
        switch (mode) {
            case "sys":
                this.mode = SYSTEM_OR_USER;
                privileged = true;
                break;
            case "usr":
                this.mode = SYSTEM_OR_USER;
                privileged = false;
                break;
            case "irq":
                this.mode = INTERRUPT_REQUEST;
                break;
            case "svc":
                this.mode = SUPERVISOR;
                break;
            case "abt":
                this.mode = ABORT;
                break;
            case "und":
                this.mode = UNDEFINED;
                break;
            default:
                throw new IndexOutOfBoundsException(mode);
        }
    }
}