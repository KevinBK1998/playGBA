run: playGBA
	./playGBA key_demo.gba
run-default: playGBA
	./playGBA
clean: playGBA
	rm -f playGBA
playGBA: playGBA.cpp ArmCpu.cpp ThumbCpu.cpp Memory.cpp Registers.cpp ArmInstructions/Instruction.cpp ThumbInstructions/Instruction.cpp ArmInstructions/Branch.h ArmInstructions/ALU.h ArmInstructions/SingleDataTransfer.h ArmInstructions/MultipleDataTransfer.h
	g++ playGBA.cpp -o playGBA
