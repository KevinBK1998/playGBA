run: playGBA
	./playGBA key_demo.gba
run-default: playGBA
	./playGBA
view-palette: palette
	./palette
view-tileset: tileset
	./tileset
clean: playGBA
	rm -f playGBA
playGBA: CommonDS.h playGBA.cpp ArmCpu.cpp ThumbCpu.cpp GPU.cpp APU.cpp Memory.cpp Registers.cpp IORegisters.cpp ArmInstructions/* ThumbInstructions/*
	g++ playGBA.cpp -o playGBA -lsfml-graphics -lsfml-window -lsfml-system
palette: palRam.bin paletteMap.cpp
	g++ paletteMap.cpp -o palette -lsfml-graphics -lsfml-window -lsfml-system
tileset: vram.bin bgControl.bin bgMap.cpp
	g++ bgMap.cpp -o tileset -lsfml-graphics -lsfml-window -lsfml-system

