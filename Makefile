run: playGBA
	./playGBA key_demo.gba
run-default: playGBA
	./playGBA
clean: playGBA
	rm -f playGBA
playGBA: CommonDS.h playGBA.cpp ArmCpu.cpp ThumbCpu.cpp GPU.cpp APU.cpp Memory.cpp Registers.cpp IORegisters.cpp ArmInstructions/* ThumbInstructions/*
	g++ playGBA.cpp -o playGBA -lsfml-graphics -lsfml-window -lsfml-system
palette: palRam.bin paletteMap.cpp
	g++ paletteMap.cpp -o paletteMap -lsfml-graphics -lsfml-window -lsfml-system
	./paletteMap
