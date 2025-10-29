#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <cstring> 

#include "c8vm.hpp"
#include "defs.hpp"
#include <SDL2/SDL.h>

uint8_t FONT_SPRITES[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void VM::setPC(uint16_t pc){
    this->PC = pc;
}

uint16_t VM::getPC(){
    return this->PC;
}

void VM::inicializar(uint16_t pcInicial){
    this->PC = pcInicial;
    this->I = 0;
    this->SP = 0;

    for(int i=0; i<16; i++) this->V[i] = 0;
    for(int i=0; i<4096; i++) this->RAM[i] = 0;
    this->DISPLAY.clear();
    for(int i=0;i<16;i++) this->stack[i] = 0;

    for(int i=0; i<16; i++) this->keypad[i] = 0; 
    this->delay_timer = 0;                      
    this->sound_timer = 0;                      

    memcpy(&this->RAM[0x000], FONT_SPRITES, 80); 
}

void VM::carregarROM(char* arqRom, uint16_t pcInicial){

  FILE* rom = fopen(arqRom, "rb");
  
  if(rom == NULL) {
    printf("Erro ao abrir o arquivo %s\n", arqRom);
    return;
  }
  
  fseek(rom, 0, SEEK_END);
  int tam_rom = ftell(rom);
  rewind(rom);
  
  if(tam_rom > (4096 - pcInicial)) {
    printf("Erro: ROM muito grande para caber na memoria\n");
    fclose(rom);
    return;
  }

  fread(&this->RAM[pcInicial], sizeof(uint8_t), tam_rom, rom);
  fclose(rom);
}


void VM::executarInstrução(){
    uint16_t instrucao = (this->RAM[this->PC] << 8 | this->RAM[this->PC + 1]);
                                                                       
    // printf("RAM: %02X, RAM + 1 %02X, PC: %04X \n", this->RAM[this->PC], this->RAM[this->PC + 1], this->PC);
    // printf("Instrucao: 0x%04X\n", instrucao);
    
    this->PC += 2;

    uint8_t grupo = instrucao >> 12;
    uint8_t X     = (instrucao & 0x0F00) >> 8;
    uint8_t Y     = (instrucao & 0x00F0) >> 4;
    uint8_t N     = instrucao & 0x000F;
    uint8_t NN    = instrucao & 0x00FF;
    uint16_t NNN   = instrucao & 0x0FFF;

    switch(grupo){

        case 0: 
            if(instrucao == 0x00E0){ // CLS
                this->DISPLAY.clear();
                break;
            }
            if(instrucao == 0x00EE){ // RET
                this->SP--;
                this->PC = this->stack[this->SP];
                break;
            }
            // Ignora 0NNN (SYS addr)
            break;

        case 1: // JP addr
            this->PC = NNN;
            break;
        
        case 2: // CALL addr
            this->stack[this->SP] = this->PC;
            this->SP++;
            this->PC = NNN;
            break;

        case 3: // SE Vx, byte
            if(this->V[X] == NN){
                this->PC += 2;
            }
            break;

        case 4: // SNE Vx, byte
            if(this->V[X] != NN) this->PC += 2;
            break;

        case 5: // SE Vx, Vy
            if(this->V[X] == this->V[Y]) this->PC += 2;
            break;

        case 6: // LD Vx, byte
            this->V[X] = NN;
            break;

        case 7: // ADD Vx, byte
            this->V[X] += NN;
            break;
        
        case 8:
            switch (N) {
                case 0x0: // 8XY0 - LD Vx, Vy
                    this->V[X] = this->V[Y];
                    break;
                case 0x1: // 8XY1 - OR Vx, Vy
                    this->V[X] = this->V[X] | this->V[Y];
                    break;
                case 0x2: // 8XY2 - AND Vx, Vy
                    this->V[X] = this->V[X] & this->V[Y];
                    break;
                case 0x3: // 8XY3 - XOR Vx, Vy
                    this->V[X] = this->V[X] ^ this->V[Y];
                    break;
                case 0x4: // 8XY4 - ADD Vx, Vy
                {
                    uint16_t soma = this->V[X] + this->V[Y];
                    this->V[0xF] = (soma > 255) ? 1 : 0; // Carry Flag 
                    this->V[X] = (uint8_t)soma;
                    break;
                }
                case 0x5: // 8XY5 - SUB Vx, Vy
                    this->V[0xF] = (this->V[X] > this->V[Y]) ? 1 : 0; // NOT Borrow
                    this->V[X] = this->V[X] - this->V[Y];
                    break;
                case 0x6: // 8XY6 - SHR Vx {, Vy}
                    this->V[0xF] = this->V[X] & 0x1; // Bit menos significativo 
                    this->V[X] = this->V[X] >> 1;
                    break;
                case 0x7: // 8XY7 - SUBN Vx, Vy
                    this->V[0xF] = (this->V[Y] > this->V[X]) ? 1 : 0; // NOT Borrow 
                    this->V[X] = this->V[Y] - this->V[X];
                    break;
                case 0xE: // 8XYE - SHL Vx {, Vy}
                    this->V[0xF] = (this->V[X] & 0x80) ? 1 : 0; // Bit mais significativo 
                    this->V[X] = this->V[X] << 1;
                    break;
                default:
                    printf("Instrucao 8XXX desconhecida: 0x%04X\n", instrucao);
                    break;
            }
            break;

        case 9: // SNE Vx, Vy
            if(this->V[X] != this->V[Y]) this->PC += 2;
            break;

        case 0xA: // LD I, addr
            this->I = NNN;
            break;

        case 0xB: // JP V0, addr
            this->PC = NNN + this->V[0];
            break;
        
        case 0xC: // RND Vx, byte
            this->V[X] = (rand() % 256) & NN; // rand() % 256 para cobrir 0-255
            break;

        case 0xD: { // DRW Vx, Vy, nibble
            uint8_t posX = this->V[X] % VIDEO_WIDTH; 
            uint8_t posY = this->V[Y] % VIDEO_HEIGHT;

            this->V[0xF] = 0; // Flag de colisão [cite: 32]

            for(int row = 0; row < N; row++){
                uint8_t spriteByte = this->RAM[this->I + row];
                for(int col = 0; col < 8; col++){
                    bool spritePixel = (spriteByte & (0x80 >> col)) != 0;
                    if(spritePixel){
                        int sx = (posX + col) % VIDEO_WIDTH;
                        int sy = (posY + row) % VIDEO_HEIGHT;
                        uint8_t prev = this->DISPLAY.togglePixel(sx, sy);
                        if(prev) this->V[0xF] = 1; // Flag de colisão 
                    }
                }
            }
            break;
        }

        case 0xE:
            switch (NN) {
                case 0x9E: // EX9E - Pula se a tecla em Vx está pressionada
                    if (pressionado(this->V[X])) {
                        this->PC += 2;
                    }
                    break;
                case 0xA1: // EXA1 - Pula se a tecla em Vx NÃO está pressionada
                    if (!pressionado(this->V[X])) {
                        this->PC += 2;
                    }
                    break;
                default:
                    printf("Instrucao EXXX desconhecida: 0x%04X\n", instrucao);
                    break;
            }
            break;

        case 0xF:
            switch (NN) {
                case 0x07: // FX07 - LD Vx, DT (Delay Timer) 
                    this->V[X] = this->delay_timer;
                    break;
                case 0x0A: // FX0A - LD Vx, K (Espera tecla e armazena em Vx) 
                {
                    bool teclaPressionada = false;
                    for (int i = 0; i < 16; i++) {
                        if (pressionado(i)) { // Usa a função 'pressionado'
                            this->V[X] = i;
                            teclaPressionada = true;
                            break;
                        }
                    }
                    if (!teclaPressionada) {
                        this->PC -= 2; // "Pausa" a execução, repete a instrução
                    }
                    break;
                }
                case 0x15: // FX15 - LD DT, Vx [cite: 104]
                    this->delay_timer = this->V[X];
                    break;
                case 0x18: // FX18 - LD ST, Vx (Sound Timer) 
                    this->sound_timer = this->V[X];
                    break;
                case 0x1E: // FX1E - ADD I, Vx
                    this->I += this->V[X];
                    break;
                case 0x29: // FX29 - LD F, Vx
                    this->I = (this->V[X] * 5);
                    break;
                case 0x33: // FX33 - LD B, Vx (BCD)
                    this->RAM[this->I]     = (this->V[X] / 100) % 10; 
                    this->RAM[this->I + 1] = (this->V[X] / 10) % 10;  
                    this->RAM[this->I + 2] = (this->V[X] / 1) % 10;   
                    break;
                case 0x55: // FX55 - LD [I], Vx (Salva V0 a VX)
                    for (int i = 0; i <= X; i++) {
                        this->RAM[this->I + i] = this->V[i];
                    }
                    break;
                case 0x65: // FX65 - LD Vx, [I] (Lê V0 a VX)
                    for (int i = 0; i <= X; i++) {
                        this->V[i] = this->RAM[this->I + i];
                    }
                    break;
                default:
                    printf("Instrucao FXXX desconhecida: 0x%04X\n", instrucao);
                    break;
            }
            break;

        default:
            printf("Grupo nao implementado! Instrucao: 0x%04X,\n Grupo: %X,\n Resto: %04X\n", instrucao, grupo, NNN);
            exit(1);
    }
}


void VM::imprimirRegistradores(){
    printf("PC: 0x%04X I: 0x%04X SP: 0x%02x\n", this->PC, this->I, this->SP);
    for(int i = 0; i < 16; i++){
        printf("V[%X]: 0x%02X ", i, this->V[i]);
    }
    printf("\n");
}


bool VM::pressionado(uint8_t key){
    if(key < 16){
        return this->keypad[key]; 
    }
    return false; 
}


void VM::renderizarTela(SDL_Renderer* renderer, int escala) {
    this->DISPLAY.render(renderer, escala);
}