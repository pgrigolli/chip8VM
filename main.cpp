#include "c8vm.hpp"
#include "defs.hpp"
//int main(int argc, char** argv){
//    VM vm = VM();

//    vm.inicializar(0x200);
    
//    vm.carregarROM(&vm, argv[1], 0x200);

//    #ifdef DEBUG
//    vm.imprimirRegistradores(&vm);
//    #endif
//    int i = 0;
//    while(i < 10){
//        vm.executarInstrução(&vm);
//        #ifdef DEBUG
//        vm.imprimirRegistradores(&vm);
//        #endif
//        i++;
//    }

//}


#include <stdio.h>
#include "c8vm.hpp"
#include "defs.hpp"
#include <SDL2/SDL.h> // Necessário para a janela

// Define a escala da janela. Cada pixel do Chip-8 (64x32)
// será desenhado como um quadrado de ESCALA x ESCALA pixels.
const int ESCALA = 10; 

int main(int argc, char** argv){
    
    // --- Verificação de Argumentos ---
    if (argc < 2) {
        printf("Uso: %s <caminho_para_rom>\n", argv[0]);
        return 1;
    }

    // --- Inicialização da VM (como antes) ---
    VM vm = VM();
    vm.inicializar(0x200);
    vm.carregarROM(&vm, argv[1], 0x200);

    // --- Inicialização do SDL ---
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erro ao inicializar SDL: %s\n", SDL_GetError());
        return 1;
    }

    // Cria a janela
    SDL_Window* window = SDL_CreateWindow(
        "Chip-8 Emulator",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        VIDEO_WIDTH * ESCALA,  // Largura da janela (ex: 64 * 10)
        VIDEO_HEIGHT * ESCALA, // Altura da janela (ex: 32 * 10)
        SDL_WINDOW_SHOWN
    );

    if (window == NULL) {
        printf("Erro ao criar janela: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Cria o renderizador
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (renderer == NULL) {
        printf("Erro ao criar renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // --- Loop Principal da Emulação ---
    bool running = true;
    SDL_Event event;

    while (running) {
        
        // 1. Processar Eventos (Input)
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            // Aqui você adicionará o mapeamento do teclado (KEYDOWN/KEYUP)
            // para atualizar o array `keypad` da VM.
        }

        // 2. Executar Ciclos da CPU
        // Executa várias instruções por frame para controlar a velocidade.
        // Ajuste o '10' conforme necessário.
        for (int i = 0; i < 10; ++i) {
            vm.executarInstrução(&vm);
        }

        // 3. Renderizar a Tela
        // Chama a nova função que você criou
        vm.renderizarTela(renderer, ESCALA);

        // 4. Atualizar Timers (delay e sound)
        // (Você precisará implementar a lógica de decremento dos timers aqui,
        // geralmente a 60Hz)

        // 5. Delay
        // Trava o loop para rodar a ~60 quadros por segundo (1000ms / 60fps ≈ 16ms)
        SDL_Delay(16);
    }

    // --- Finalização ---
    printf("Encerrando emulador.\n");
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}