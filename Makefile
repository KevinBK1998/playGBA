run: playGBA
	./playGBA TEMP.gb
run-default: playGBA
	./playGBA
clean: playGBA
	rm -f playGBA
playGBA: playGBA.cpp ArmCpu.cpp ArmInstruction.cpp Memory.cpp Registers.cpp
	g++ playGBA.cpp -o playGBA
