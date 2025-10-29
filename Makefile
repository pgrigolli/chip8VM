all: chip8

CC = g++
CFLAGS = -std=c++11 -O2
LDFLAGS = -lSDL2

chip8: c8vm.o main.o display.o
	$(CC) $(CFLAGS) -o $@ c8vm.o main.o display.o $(LDFLAGS)

c8vm.o: c8vm.cpp c8vm.hpp defs.hpp display.hpp
	$(CC) $(CFLAGS) -c c8vm.cpp -o c8vm.o

main.o: main.cpp c8vm.hpp defs.hpp
	$(CC) $(CFLAGS) -c main.cpp -o main.o

display.o: display.cpp display.hpp defs.hpp
	$(CC) $(CFLAGS) -c display.cpp -o display.o

.PHONY: run
run: all
	@if [ -z "$(ROM)" ]; then \
		echo "Uso: make run ROM=<caminho_para_rom>"; \
		exit 1; \
	fi
	./chip8 "$(ROM)"

.PHONY: clean
clean:
	-rm -f *.o chip8

.PHONY: help
help:
	@echo "Targets disponíveis:"
	@echo "  all       - Compila o executável (padrão)"
	@echo "  run       - Executa o emulador com uma ROM (ex: make run ROM=roms/MAZE)"
	@echo "  clean     - Remove arquivos objeto e executável"
	@echo "  help      - Mostra esta ajuda"