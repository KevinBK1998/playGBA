run: playGBA
	./playGBA TEMP.gb
run-default: playGBA
	./playGBA
playGBA: playGBA.cpp
	g++ playGBA.cpp -o playGBA
