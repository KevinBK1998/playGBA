run: playGBA
	./playGBA key_demo.gba
run-default: playGBA
	./playGBA
view-palette: palette palRam.bin
	./palette
view-tileset: tileset vram.bin bgControl.bin
	./tileset
clean: playGBA
	rm -f playGBA
playGBA: CommonDS.h playGBA.cpp ArmCpu.cpp ThumbCpu.cpp GPU.cpp APU.cpp Memory.cpp Registers.cpp IORegisters.cpp ArmInstructions/* ThumbInstructions/*
	g++ playGBA.cpp -o playGBA -lsfml-graphics -lsfml-window -lsfml-system
palette: paletteMap.cpp
	g++ paletteMap.cpp -o palette -lsfml-graphics -lsfml-window -lsfml-system
tileset: bgMap.cpp
	g++ bgMap.cpp -o tileset -lsfml-graphics -lsfml-window -lsfml-system

