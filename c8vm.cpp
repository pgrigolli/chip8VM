#include "c8vm.hpp"
#include <stdio.h>
#include <stdlib.h> 

void VM::setPC(uint16_t pc){
    this->PC = pc;
}

uint16_t VM::getPC(){
    return this->PC;
}

void VM::inicializar(uint16_t pcInicial){
    this->setPC(pcInicial);
}

void VM::carregarROM(VM* vm, char* arqRom, uint16_t pcInicial){

    FILE* rom = fopen(arqRom, "rb");
    fseek(rom, 0, SEEK_END);
    int tamRom = ftell(rom);
    rewind(rom);

    fread(&vm->RAM[pcInicial], 1, tamRom, rom);

    fclose(rom);
}

void VM::executarInstrução(VM* vm){
    uint16_t instrucao = (vm->RAM[vm->PC] << 8 | vm->RAM[vm->PC + 1]); //Coloca a instrução em PC na nos 4 bits mais a esquerda e
                                                                       //Coloca a instrução em PC + 1 nos 4 bits mais a direita 

    printf("Instrucao: 0x%04X\n", instrucao);
    vm->PC += 2;

    uint8_t grupo = instrucao >> 12;
    uint8_t X     = (instrucao & 0x0F00) >> 8;
    uint8_t Y     = (instrucao & 0x00F0) >> 4;
    uint8_t N     = instrucao & 0x000F;
    uint8_t NN    = instrucao & 0x00FF;
    uint8_t NNN   = instrucao & 0x0FFF;

    switch(grupo){

        case 0:
            // CLS  
            if(instrucao == 0x00E0){
                for(int i = 0; i < 64 * 32; i++){
                    vm->DISPLAY[i] = 0;
                }
                break;
            }
            // RET
            if(instrucao == 0x00EE){
                vm->PC = vm->stack[vm->SP];
                vm->SP--;

                break;
            }

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
        case 6:
            vm->V[X] = NN;
            break;

        case 10:
            vm->I = NNN;

            break;

        case 13:
            uint8_t posX = V[X] % 64; // Posições podem vir "para fora" do display
            uint8_t posY = V[Y] % 32;

            V[15] = 0; //V[F] = 0

            for(int i = N; i < vm->RAM[vm->I + N]; i++){
                    
            }



        default:
            printf("Grupo nao implementado! Instrucao: 0x%04X\n, Grupo: %d, Resto: %d", instrucao, grupo, NNN);
            exit(1);
    }
}

void VM::imprimirRegistradores(VM* vm){
    printf("PC: 0x%04X I: 0x%04X SP: 0x%02x\n", vm->PC, vm->I, vm->V[I]);
    for(int i = 0; i < 16; i++){
        printf("V[%X]: 0x%02X ", i, vm->V[i]);
    }
    printf("\n");
}