#pragma once
#include <stdint.h>
#include "defs.hpp"
#include <SDL2/SDL.h>

class Display {
    private:
        uint8_t pixels[VIDEO_WIDTH * VIDEO_HEIGHT];
    public:
        Display();
        void clear();
        uint8_t getPixel(int x, int y) const;
        void setPixel(int x, int y, uint8_t v);
        // Toggle pixel (xor). Retorna o valor anterior (0 ou 1).
        uint8_t togglePixel(int x, int y);
        void render(SDL_Renderer* renderer, int escala) const;
};