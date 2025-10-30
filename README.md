# CHIP-8 Virtual Machine

Este projeto implementa uma máquina virtual do Chip-8, um interpretador usado originalmente em microcomputadores da década de 1970 para rodar jogos simples.
O objetivo deste trabalho é recriar a execução do sistema, interpretando instruções do Chip-8 e exibindo a saída gráfica em uma janela.

# Execução

## Compilar usando Makefile
### Pré-Requisitos
* C++
* g++
* SDL2
``` bash
make all
```

## Executar o código
Existem dois parâmetros não obrigatórios:
* --scale escala - Escala de exibição da tela - Padrão 10x
*  --clock frequencia - Frequência de execução (Instruções por segundo) - Padrão 500Hz

``` bash
./chip8 --scale <escala> --clock <frequencia> caminho/para/ROM
```
Exemplo

``` bash
./chip8 --scale 10 --clock 500 ./roms/PONG
```
