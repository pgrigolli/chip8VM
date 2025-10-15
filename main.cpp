#include "c8vm.hpp"
#include "defs.hpp"
int main(int argc, char** argv){
    VM vm = VM();

    vm.inicializar(0x200);
    
    vm.carregarROM(&vm, argv[1], 0x200);

    #ifdef DEBUG
    vm.imprimirRegistradores(&vm);
    #endif

    while(1){
        vm.executarInstrução(&vm);
        #ifdef DEBUG
        vm.imprimirRegistradores(&vm);
        #endif
    }

}