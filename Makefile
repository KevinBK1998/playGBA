run: playGBA
	./playGBA key_demo.gba
run-default: playGBA
	./playGBA
clean: playGBA
	rm -f playGBA
playGBA: playGBA.cpp ArmCpu.cpp ThumbCpu.cpp Memory.cpp Registers.cpp IORegisters.cpp ArmInstructions/Instruction.cpp ArmInstructions/Branch.h ThumbInstructions/Branch.h ArmInstructions/ALU.h ArmInstructions/SingleDataTransfer.h ArmInstructions/MultipleDataTransfer.h ThumbInstructions/*
	g++ playGBA.cpp -o playGBA
