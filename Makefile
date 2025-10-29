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

# --- Alvos de Execução ---

# Alvos "PHONY" (não são arquivos)
.PHONY: run clean

# Alvo 'run' genérico (compila e roda)
run: all
	@echo "Executando: ./chip8 --clock $(FREQUENCIA) --scale $(ESCALA) \"$(ROM)\""
	@./chip8 --clock $(FREQUENCIA) --scale $(ESCALA) "$(ROM)"

# --- Limpeza ---
clean:
	-rm -f *.o chip8




# How to execute
# make run ROM="./roms/TETRIS" ESCALA=5 FREQUENCIA=100