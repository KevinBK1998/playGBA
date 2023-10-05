class Registers
{
private:
    // Registers 0-12
    int unbankedReg[13];
    // Registers 13(SP) and 14(LR)
    int bankedReg[10];
    int savedStatusReg[5];
    int currentStatusReg;
    int reg15 = 4;
public:
    Registers(/* args */);
    ~Registers();
};

Registers::Registers(/* args */)
{
}

Registers::~Registers()
{
}
