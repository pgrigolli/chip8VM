#include <stdio.h>
#include <fstream> 
#include "c8vm.hpp"
#include "defs.hpp"
#include <SDL2/SDL.h> 

const int ESCALA = 10; 
const int FPS = 60; 
const int FRAME_DELAY = 1000 / FPS; 
const int CLOCK_HZ = 500; 
const int CICLOS_POR_FRAME = CLOCK_HZ / FPS; 


int main(int argc, char** argv){
    
    if (argc < 2) {
        printf("Uso: %s <caminho_para_rom>\n", argv[0]);
        return 1;
    }

    VM vm = VM();
    vm.inicializar(0x200);
    
    vm.carregarROM(argv[1], 0x200);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("Erro ao inicializar SDL: %s\n", SDL_GetError());
        return 1;
    }

    std::ifstream romFile(argv[1], std::ios::binary);
    if (!romFile) {
        fprintf(stderr, "Erro: Não foi possível abrir a ROM '%s'\n", argv[1]);
        return 1;
    }
    romFile.close();

    SDL_Window* window = SDL_CreateWindow(
        "Chip-8 Emulator",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        VIDEO_WIDTH * ESCALA,  // 64 * 10
        VIDEO_HEIGHT * ESCALA, // 32 * 10
        SDL_WINDOW_SHOWN
    );
    if (window == NULL) {
        printf("Erro ao criar janela %s\n", SDL_GetError());
        return 1; 
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Erro ao criar renderizador %s\n", SDL_GetError());
        return 1;
    }

    bool running = true;
    SDL_Event event;

    while (running) { // Laço principal
        
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_1: vm.keypad[0x1] = 1; break;
                    case SDLK_2: vm.keypad[0x2] = 1; break;
                    case SDLK_3: vm.keypad[0x3] = 1; break;
                    case SDLK_4: vm.keypad[0xC] = 1; break;
                    case SDLK_q: vm.keypad[0x4] = 1; break;
                    case SDLK_w: vm.keypad[0x5] = 1; break;
                    case SDLK_e: vm.keypad[0x6] = 1; break;
                    case SDLK_r: vm.keypad[0xD] = 1; break;
                    case SDLK_a: vm.keypad[0x7] = 1; break;
                    case SDLK_s: vm.keypad[0x8] = 1; break;
                    case SDLK_d: vm.keypad[0x9] = 1; break;
                    case SDLK_f: vm.keypad[0xE] = 1; break;
                    case SDLK_z: vm.keypad[0xA] = 1; break;
                    case SDLK_x: vm.keypad[0x0] = 1; break;
                    case SDLK_c: vm.keypad[0xB] = 1; break;
                    case SDLK_v: vm.keypad[0xF] = 1; break;
                    default: break;
                }
            }

            if (event.type == SDL_KEYUP) {
                switch (event.key.keysym.sym) {
                    case SDLK_1: vm.keypad[0x1] = 0; break;
                    case SDLK_2: vm.keypad[0x2] = 0; break;
                    case SDLK_3: vm.keypad[0x3] = 0; break;
                    case SDLK_4: vm.keypad[0xC] = 0; break;
                    case SDLK_q: vm.keypad[0x4] = 0; break;
                    case SDLK_w: vm.keypad[0x5] = 0; break;
                    case SDLK_e: vm.keypad[0x6] = 0; break;
                    case SDLK_r: vm.keypad[0xD] = 0; break;
                    case SDLK_a: vm.keypad[0x7] = 0; break;
                    case SDLK_s: vm.keypad[0x8] = 0; break;
                    case SDLK_d: vm.keypad[0x9] = 0; break;
                    case SDLK_f: vm.keypad[0xE] = 0; break;
                    case SDLK_z: vm.keypad[0xA] = 0; break;
                    case SDLK_x: vm.keypad[0x0] = 0; break;
                    case SDLK_c: vm.keypad[0xB] = 0; break;
                    case SDLK_v: vm.keypad[0xF] = 0; break;
                    default: break;
                }
            }
        }

        for (int i = 0; i < CICLOS_POR_FRAME; ++i) {
            vm.executarInstrução();
        }

        vm.renderizarTela(renderer, ESCALA);

        if (vm.delay_timer > 0) {
            vm.delay_timer--;
        }

        SDL_Delay(FRAME_DELAY);
    }

    printf("Encerrando emulador.\n");
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}