run: playGBA
	./playGBA TEMP.gb
run-default: playGBA
	./playGBA
playGBA: playGBA.cpp ArmCpu.cpp ArmInstruction.cpp Memory.cpp Registers.cpp
	g++ playGBA.cpp -o playGBA
