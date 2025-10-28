#pragma once 

#include <stdint.h>
#include "defs.hpp"
#include <SDL2/SDL.h>
#include "display.hpp"

class VM{
    private:
        uint8_t  RAM[4096];      // Memória de 4KB
        uint16_t PC;             // Program Counter
        uint8_t  V[16];          // Registradores de proposito geral
        uint8_t  SP;             // Stack Pointer
        uint16_t I;              // Registrador de índice
        uint16_t stack[16];                           // Pilha
        Display  DISPLAY;                              // Tela encapsulada em classe

    public:

        void setPC(uint16_t pcInicial);
        uint16_t getPC();

        void inicializar(uint16_t pcInicial);
        void executarInstrução(VM* vm);
        void carregarROM(VM* vm, char* arqRom, uint16_t pc_inicial);
        void imprimirRegistradores(VM* vm);

        bool pressionado(uint8_t key);

        void renderizarTela(SDL_Renderer* renderer, int escala);


};