run: playGBA
	./playGBA key_demo.gba
run-default: playGBA
	./playGBA
clean: playGBA
	rm -f playGBA
playGBA: playGBA.cpp ArmCpu.cpp ArmInstructions/Instruction.cpp Memory.cpp Registers.cpp ArmInstructions/ALU.h ArmInstructions/SingleDataTransfer.h ArmInstructions/MultipleDataTransfer.h
	g++ playGBA.cpp -o playGBA
