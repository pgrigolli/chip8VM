#pragma once 

#include <stdint.h>
#include "defs.hpp"
#include <SDL2/SDL.h>
#include "display.hpp" 

class VM{
    private:
        uint8_t  RAM[4096];
        uint16_t PC;
        uint8_t  V[16];
        uint8_t  SP;
        uint16_t I;
        uint16_t stack[16];
        Display  DISPLAY;

    public:
        // Membros públicos para timers e teclado
        uint8_t delay_timer;
        uint8_t sound_timer;
        uint8_t keypad[16];

        void setPC(uint16_t pcInicial);
        uint16_t getPC();

        // --- ASSINATURAS CORRIGIDAS (Estilo C++) ---
        void inicializar(uint16_t pcInicial);
        void executarInstrução(); // Removido (VM* vm)
        void carregarROM(char* arqRom, uint16_t pc_inicial); // Removido (VM* vm)
        void imprimirRegistradores(); // Removido (VM* vm)
        
        bool pressionado(uint8_t key);
        void renderizarTela(SDL_Renderer* renderer, int escala);
};