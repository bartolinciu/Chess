all:
	g++ main.cpp renderer.cpp chess.cpp sprite.cpp -Wall -Wextra -o chess.exe -lSDL2 -lSDL2_image
test:all
	chess.exe