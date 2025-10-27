all: chip8

CC = g++
CFLAGS = -std=c++11 -O2
LDFLAGS = -lSDL2


chip8: c8vm.o main.o
	$(CC) $(CFLAGS) -o $@ c8vm.o main.o $(LDFLAGS) 

c8vm.o: c8vm.cpp c8vm.hpp defs.hpp
	$(CC) $(CFLAGS) -c c8vm.cpp -o c8vm.o

main.o: main.cpp c8vm.hpp defs.hpp
	$(CC) $(CFLAGS) -c main.cpp -o main.o

.PHONY: run
run: all
	./chip8 "IBM Logo.ch8"

.PHONY: clean
clean:
	-rm -f *.o chip8