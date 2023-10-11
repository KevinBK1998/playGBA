run: playGBA
	./playGBA key_demo.gba
run-default: playGBA
	./playGBA
clean: playGBA
	rm -f playGBA
playGBA: playGBA.cpp ArmCpu.cpp ThumbCpu.cpp Memory.cpp Registers.cpp IORegisters.cpp ArmInstructions/* ThumbInstructions/*
	g++ playGBA.cpp -o playGBA
