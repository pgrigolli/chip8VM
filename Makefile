all: chip8

CC = g++
CFLAGS = -std=c++11 -O2
LDFLAGS = -lSDL2

# --- Variáveis de Execução (com valores padrão) ---
# Altere na linha de comando:
# make run ROM="./roms/TETRIS" ESCALA=5 FREQUENCIA=100
ROM        ?= "./roms/IBM Logo.ch8"
ESCALA     ?= 10
FREQUENCIA ?= 500 # 'FREQUENCIA' é o argumento --clock


chip8: c8vm.o main.o display.o
	$(CC) $(CFLAGS) -o $@ c8vm.o main.o display.o $(LDFLAGS)

c8vm.o: c8vm.cpp c8vm.hpp defs.hpp display.hpp
	$(CC) $(CFLAGS) -c c8vm.cpp -o c8vm.o

main.o: main.cpp c8vm.hpp defs.hpp
	$(CC) $(CFLAGS) -c main.cpp -o main.o

display.o: display.cpp display.hpp defs.hpp
	$(CC) $(CFLAGS) -c display.cpp -o display.o

	@echo "Compilação concluída. Use 'make run ROM=\"caminho/para/rom.ch8\"' para executar."
	@echo "Parâmetros padrão: ESCALA=$(ESCALA), FREQUENCIA=$(FREQUENCIA)"
	@echo "Uso: ./chip8 --clock <frequencia> --scale <escala> <caminho/para/rom.ch8>"
	@echo "Targets disponíveis:"
	@echo "  make run       - Executa o emulador com uma ROM com os parâmetros padrão(ex: make run ROM=roms/MAZE)"
	@echo "  make clean     - Remove arquivos objeto e executável"
.PHONY: run clean

run: all
	@echo "Executando: ./chip8 --clock $(FREQUENCIA) --scale $(ESCALA) \"$(ROM)\""
	@./chip8 --clock $(FREQUENCIA) --scale $(ESCALA) "$(ROM)"

clean:
	-rm -f *.o chip8