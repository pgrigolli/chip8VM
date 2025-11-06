#include <stdio.h>
#include <cstring>  
#include <cstdlib>  
#include "c8vm.hpp"
#include "defs.hpp"
#include <SDL2/SDL.h> 

// --- Valores Padrão ---
// Deixam de ser 'const' para podermos alterá-los
int ESCALA = 10;        // Padrão 
int CLOCK_HZ = 500;   // Padrão 
uint16_t LOAD_ADDRESS = 0x200; // Padrão 

const int FPS = 60; // Tela e timers rodam a 60Hz 
const int FRAME_DELAY = 1000 / FPS; 
// int CICLOS_POR_FRAME; // Será calculado DEPOIS da análise

/**
 * @brief Imprime as instruções de uso conforme especificado 
 */
void imprimirAjuda(char* nomePrograma) {
    printf("Uso: %s [opções] caminho/para/a/rom.ch8\n", nomePrograma); 
    printf("\nOpções de Linha de Comando:\n"); 
    printf("  --clock <velocidade>  Define a velocidade da CPU em Hz. Padrão: 500Hz.\n");
    printf("  --scale <fator>       Define o fator de escala da janela. Padrão: 10.\n");
    printf("  --load <endereco>     Define o endereço de carga da ROM (ex: 512 ou 0x200). Padrão: 0x200. \n");
    printf("  --help                Exibe esta mensagem de ajuda.\n");
}


int main(int argc, char** argv){
    
    if (argc < 2) {
        imprimirAjuda(argv[0]);
        return 1;
    }

    char* romPath = NULL;

    // --- 1. Análise dos Argumentos de Linha de Comando ---
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            imprimirAjuda(argv[0]);
            return 0; // Sai do programa com sucesso
        } 
        else if (strcmp(argv[i], "--clock") == 0) { 
            if (i + 1 < argc) {
                CLOCK_HZ = atoi(argv[++i]); // Converte o próximo argumento para int
            } else {
                printf("Erro: --clock requer um valor.\n");
                return 1;
            }
        } 
        else if (strcmp(argv[i], "--scale") == 0) { 
            if (i + 1 < argc) {
                ESCALA = atoi(argv[++i]); // Converte o próximo argumento para int
            } else {
                printf("Erro: --scale requer um valor.\n");
                return 1;
            }
        } 
        else if (strcmp(argv[i], "--load") == 0) { 
             if (i + 1 < argc) {
                // strtol (base 0) lida com "512" e "0x200"
                LOAD_ADDRESS = (uint16_t)strtol(argv[++i], NULL, 0); 
            } else {
                printf("Erro: --load requer um valor.\n");
                return 1;
            }
        }
        else if (argv[i][0] == '-') {
            printf("Erro: Opção desconhecida '%s'\n", argv[i]);
            imprimirAjuda(argv[0]);
            return 1;
        } 
        else {
            // Se não for uma opção, deve ser o caminho da ROM
            if (romPath == NULL) {
                romPath = argv[i];
            } else {
                printf("Erro: Múltiplos caminhos de ROM especificados.\n");
                imprimirAjuda(argv[0]);
                return 1;
            }
        }
    }

    // Verifica se a ROM foi fornecida
    if (romPath == NULL) {
        printf("Erro: Caminho da ROM não especificado.\n");
        imprimirAjuda(argv[0]);
        return 1;
    }

    // --- 2. Calcular CICLOS_POR_FRAME com base nos argumentos ---
    const int CICLOS_POR_FRAME = CLOCK_HZ / FPS; 
    
    printf("Configurações:\n  ROM: %s\n  Clock: %d Hz\n  Escala: %dx\n  Endereço Carga: 0x%X\n",
           romPath, CLOCK_HZ, ESCALA, LOAD_ADDRESS);
    printf("Executando %d ciclos de CPU por frame.\n", CICLOS_POR_FRAME);


    // --- 3. Inicialização da VM (usando as variáveis) ---
    VM vm = VM();
    vm.inicializar(LOAD_ADDRESS); 
    vm.carregarROM(romPath, LOAD_ADDRESS); 

    // --- 4. Inicialização do SDL (usando as variáveis) ---
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("Erro ao inicializar SDL: %s\n", SDL_GetError());
        return 1;
    }

    // Cria a janela (usando a ESCALA configurada)
    SDL_Window* window = SDL_CreateWindow(
        "Chip-8 Emulator",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        VIDEO_WIDTH * ESCALA,  // Usa a variável
        VIDEO_HEIGHT * ESCALA, // Usa a variável
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

    // --- Loop Principal da Emulação (sem alterações) ---
    bool running = true;
    SDL_Event event;

    while (running) { // Laço principal
        
        // 1. Processar Eventos (Input) 
        while (SDL_PollEvent(&event)) {
            // ... (seu código de teclado)
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

        // 2. Executar Ciclos da CPU (a X Hz) 
        for (int i = 0; i < CICLOS_POR_FRAME; ++i) {
            vm.executarInstrução(); 
        }

        // 3. Renderizar a Tela (a 60Hz) 
        vm.renderizarTela(renderer, ESCALA); // Passa a ESCALA

        // 4. Atualizar Timers (a 60Hz) 
        if (vm.delay_timer > 0) {
            vm.delay_timer--;
        }

        if (vm.sound_timer > 0) {
            vm.sound_timer--;
            // TODO: Adicionar lógica de "BEEP" 
        }

        // 5. Delay
        SDL_Delay(FRAME_DELAY);
    }

    printf("Encerrando emulador.\n");
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}