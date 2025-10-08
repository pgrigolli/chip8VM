all:
	gpp -c c8vm.cpp
	gpp -c main.cpp
	gpp -o chip8 c8vm.o main.o