run: playGBA
	./playGBA key_demo.gba
run-default: playGBA
	./playGBA
clean: playGBA
	rm -f playGBA
playGBA: playGBA.cpp ArmCpu.cpp ThumbCpu.cpp Memory.cpp Registers.cpp ArmInstructions/Instruction.cpp ThumbInstructions/Instruction.cpp ArmInstructions/Branch.h ThumbInstructions/Branch.h ThumbInstructions/LongBranch.h ArmInstructions/ALU.h ThumbInstructions/ALU.h ArmInstructions/SingleDataTransfer.h ThumbInstructions/SingleDataTransfer.h ArmInstructions/MultipleDataTransfer.h ThumbInstructions/MultipleDataTransfer.h ThumbInstructions/ShiftedALU.h
	g++ playGBA.cpp -o playGBA
