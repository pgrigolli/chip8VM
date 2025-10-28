#include "display.hpp"
#include <cstring>

Display::Display() {
    clear();
}

void Display::clear() {
    std::memset(pixels, 0, sizeof(pixels));
}

uint8_t Display::getPixel(int x, int y) const {
    if (x < 0 || x >= VIDEO_WIDTH || y < 0 || y >= VIDEO_HEIGHT) return 0;
    return pixels[y * VIDEO_WIDTH + x];
}

void Display::setPixel(int x, int y, uint8_t v) {
    if (x < 0 || x >= VIDEO_WIDTH || y < 0 || y >= VIDEO_HEIGHT) return;
    pixels[y * VIDEO_WIDTH + x] = v ? 1 : 0;
}

uint8_t Display::togglePixel(int x, int y) {
    if (x < 0 || x >= VIDEO_WIDTH || y < 0 || y >= VIDEO_HEIGHT) return 0;
    int idx = y * VIDEO_WIDTH + x;
    uint8_t prev = pixels[idx];
    pixels[idx] ^= 1;
    return prev;
}

void Display::render(SDL_Renderer* renderer, int escala) const {
    // Limpa a tela (fundo preto)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Define a cor dos pixels "acesos" (branco)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    for (int y = 0; y < VIDEO_HEIGHT; y++) {
        for (int x = 0; x < VIDEO_WIDTH; x++) {
            if (getPixel(x, y)) {
                SDL_Rect pixel = { x * escala, y * escala, escala, escala };
                SDL_RenderFillRect(renderer, &pixel);
            }
        }
    }

    SDL_RenderPresent(renderer);
}