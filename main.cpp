// -----------------------------------------
// Programa principal MAIN
// -----------------------------------------


// -----------------------------------------
// SEÇÃO HEADER (.h)
// -----------------------------------------

// #define DEBUG_ENABLED

// #include "conversorADFinal.c"
// #include "lcdFinal.c"
#include "ProjetoFinal.c"

// -----------------------------------------
// SEÇÃO DE IMPLEMENTAÇÃO (.c)
// -----------------------------------------


#ifndef Arduino_h
int main(void) {
#ifdef DEBUG_ENABLED
    debug_init(); // inicializa a estrutura de debug
    sei();    // habilita as interrupções
#endif
    setup();
    while(1) loop();
    return 0;
}
#endif
