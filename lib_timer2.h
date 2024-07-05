// ------------------------------------------------------
// Biblioteca do Timer 0
// ------------------------------------------------------


// [====================================================]
// [                    HEADER (.h)                     ]
// [====================================================]

#ifndef lib_timer2_h
#define lib_timer2_h

#include "lib_arduino.h"    // define bitSet,Clear,Toggle,Read e delay
// #include "lib_pinIO_Uno.h"  // define funções configPin; pinSet,Clear,Toggle,Read
// #include "lib_shieldMF.h"   // configura o shield


// protótipos das funções
void setupTimer2();


#define T2_CompMatchA 0 // 0:Normal, 1:Toggle OC0A, 2:Clear OC0A, 3:Set OC0A
#define T2_CompMatchB 0 // 0:Normal, 1:Toggle OC0B, 2:Clear OC0B, 3:Set OC0B
#define T2_WGM 0 // 0:Normal , 1:PWM phase correct 0xFF, 2: CTC OCR0A, 3:Fast PWM 0xFF, 5:PWM phase correct OCR0A, 7:Fast PWM OCR0A
#define T2_ForceA 0 // 0:no force, 1:force
#define T2_ForceB 0 // 0:no force, 1:force
#define T2_Prescaler 4 // 0:no clock, 1:div 1, 2:div 8, 3:div 64, 4:div256, 5:div 1024, 6:external borda descida, 7: external borda subida

#define T2_clearOverflowFlag() bitSet(TIFR2, TOV2)
#define T2_enableInterruptOV() bitSet(TIMSK2,TOIE2)
#define T2_interruptEnableCMPA() bitSet(TIMSK2,OCIE2A)

// [====================================================]
// [                 IMPLEMENTATION (.c)                ]
// [====================================================]


void setupTimer2() {
    // TCCR0A = 0b00000000;
    // TCCR0B = 0b00000010; // div 8
    TCCR2A = (T2_CompMatchA<<6) | (T2_CompMatchB<<4) | (T2_WGM&0b11);
    TCCR2B = (T2_ForceA<<7) | (T2_ForceB<<6) | ((T2_WGM&0b100)<<1) | (T2_Prescaler);
    bitClear(PRR0,PRTIM2); // liga a energia do Timer 2  
}

#endif // Finaliza lib_timer2_h

// SOLUÇÃO:
// - Criar uma estrutura de defines
// - Fazer a montagem dos bits com shifts e máscaras lógicas


// PROBLEMA:
// - temos que ter o datasheet sempre aberto
// - temos que olhar as tabelas para saber quais bits colocar no registrador
