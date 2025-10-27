#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>

#include "c8vm.hpp"
#include "defs.hpp"

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

    for(int i=0; i<16; i++)this->V[i] = 0;
    for(int i=0; i<4096; i++) this->RAM[i] = 0;
    for(int i=0;i<VIDEO_WIDTH*VIDEO_HEIGHT;i++) this->DISPLAY[i] = 0;
    for(int i=0;i<16;i++) this->stack[i] = 0;
}

void VM::carregarROM(VM* vm, char* arqRom, uint16_t pcInicial){

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

void VM::executarInstrução(VM* vm){
    uint16_t instrucao = (vm->RAM[vm->PC] << 8 | vm->RAM[vm->PC + 1]); //Coloca a instrução em PC na nos 4 bits mais a esquerda e
                                                                       //Coloca a instrução em PC + 1 nos 4 bits mais a direita 
    printf("RAM: %d, RAM + 1 %d \n", vm->RAM[vm->PC], vm->RAM[vm->PC + 1]);
    printf("Instrucao: 0x%04X\n", instrucao);
    
    uint8_t grupo = instrucao >> 12;
    uint8_t X     = (instrucao & 0x0F00) >> 8;
    uint8_t Y     = (instrucao & 0x00F0) >> 4;
    uint8_t N     = instrucao & 0x000F;
    uint8_t NN    = instrucao & 0x00FF;
    uint16_t NNN   = instrucao & 0x0FFF;

    switch(grupo){

        case 0: 

            // CLS  
            if(instrucao == 0x00E0){
                for(int i = 0; i < VIDEO_HEIGHT * VIDEO_WIDTH; i++){
                    vm->DISPLAY[i] = 0;
                }
                break;
            }
            // RET
            if(instrucao == 0x00EE){
                vm->PC = vm->stack[vm->SP];
                vm->SP--;
                
                break;
            }else{
                break;
            }
        
            break;

        case 1:
            // JP addr
            vm->PC = NNN;
            break;
        
        case 2:
            // CALL addr
            vm->SP++;
            vm->stack[vm->SP] = vm->PC;
            vm->PC = NNN;

            break;
        case 3:
            // Skip if Equal (SE)
            if(vm->V[X] == NN){
                vm->PC += 2;
            }

            break;

        case 4:
            if(vm->V[X] != NN) vm->PC += 2;
            break;

        case 5:
            if(vm->V[X] == vm->V[Y]) vm->PC += 2;
            break;

        case 6:
            vm->V[X] = NN;
            break;

        case 7:
            vm->V[X] += NN;
            break;
        
        case 8:
            vm->V[X] += vm->V[Y];
            if(vm->V[X] > 255) vm->V[0xF] = 1;
            break;

        case 9:
            if(vm->V[X] != vm->V[Y]) vm->PC += 2;

        case 0xA:
            vm->I = NNN;

            break;

        case 0xB:
            vm->PC = NNN + vm->V[0];
        
        case 0xC:
            vm->V[X] = (rand() % 255) & NN;


        case 0xD: {
            uint8_t posX = vm->V[X] % VIDEO_WIDTH; 
            uint8_t posY = vm->V[Y] % 32;

            vm->V[0xF] = 0;

            for(int row = 0; row < N; row++){
                    
                uint8_t spriteByte = vm->RAM[vm->I + row];

                for(int col = 0; col < 8; col++){
                    uint8_t spritePixel = spriteByte & (0x80 >> col);
                    uint32_t screenIndex = (posY + row) * VIDEO_WIDTH + (posX + col);
                    uint32_t screenPixel = vm->DISPLAY[screenIndex];

                    if (spritePixel){
                        if (screenPixel == 0xFFFFFFFF) vm->V[0xF] = 1;
                        vm->DISPLAY[screenIndex] = screenPixel ^ 0xFFFFFFFF;
                    }
                }
            }
            break;
        }

        case 0xE:
            if(pressionado(vm->V[X])) vm->PC += 2;
            break;

        case 0xF:
            break;

        default:
            printf("Grupo nao implementado! Instrucao: 0x%04X,\n Grupo: %X,\n Resto: %04X\n", instrucao, grupo, NNN);
            exit(1);
    }

    vm->PC += 2;

}

void VM::imprimirRegistradores(VM* vm){
    printf("PC: 0x%04X I: 0x%04X SP: 0x%02x\n", vm->PC, vm->I, vm->V[SP]);
    for(int i = 0; i < 16; i++){
        printf("V[%X]: 0x%02X ", i, vm->V[i]);
    }
    printf("\n");
}

bool VM::pressionado(uint8_t key){
    // if(key < 16){
    //     return keypad[key];
    // } 
    return false;
}