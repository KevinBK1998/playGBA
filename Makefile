run: playGBA
	./playGBA key_demo.gba
run-default: playGBA
	./playGBA
clean: playGBA
	rm -f playGBA
playGBA: CommonDS.h playGBA.cpp ArmCpu.cpp ThumbCpu.cpp GPU.cpp APU.cpp Memory.cpp Registers.cpp IORegisters.cpp ArmInstructions/* ThumbInstructions/*
	g++ playGBA.cpp -o playGBA
