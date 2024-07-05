// ------------------------------------------------------
// Biblioteca do Shield Multifunção
// ------------------------------------------------------


// [====================================================]
// [                    HEADER (.h)                     ]
// [====================================================]


//  PIN   PORT  I/O Description
// ------ ----  --- --------------------
//   8     PB0   O  74HC595-SDI
//   9     PB1   ?  PWM/OC1A (livre)
//  10     PB2   O  led 1
//  11     PB3   O  led 2 / MOSI (SPI)
//  12     PB4   O  led 3 / MISO (SPI)
//  13     PB5   O  led 4 / SCK (SPI)
//  A0,14  PC0   I  Potenciômetro
//  A1,15  PC1   I  BOT1
//  A2,16  PC2   I  BOT2
//  A3,17  PC3   I  BOT3
//  A4,18  PC4   O  A4 (I2C SDA)
//  A5,19  PC5   O  A5 (I2C SCL)
//   0     PD0   ?  APC220-5 (livre)
//   1     PD1   ?  APC220-4 (livre)
//   2     PD2   ?  Header Temp (livre)
//   3     PD3   O  Buzzer
//   4     PD4   O  74HC595-LATCHCLK
//   5     PD5   ?  T1 / OC0B (PWM) (livre)
//   6     PD6   ?  OC0A (PWM) (livre)
//   7     PD7   O  74HC595-SHIFTCLK


#ifndef lib_shield_MF       // trava múltiplas inclusões

#define lib_shield_MF


#include <avr/io.h>         // define os registradores
#include "lib_arduino.h"    // define bitSet e outras funções de bit
#include "lib_IO_Mega.h"  // mapeamento PORTA/pino funções config

// Definições dos LEDs
#define LED1 10
#define LED2 11
#define LED3 12
#define LED4 13
#define LED_COUNT 4
#define LED_OFF 0

// Definições dos botões
#define SW1 ANALOG1
#define SW2 ANALOG2
#define SW3 ANALOG3

// Definições 
#define SDI     8
#define SHCLOCK 7
#define LTCLOCK 4

// Definições dos modos
#define MODO_LEITURA 0
#define MODO_DESCANSO 1
#define MODO_FESTA 2

// Definições do potenciômetro
#define POT ANALOG0

// #define TX 1
// #define RX 0


// [====================================================]
// [                 IMPLEMENTATION (.c)                ]
// [====================================================]


void setupShieldMF() {
    configPinOutput(LED1);
    configPinOutput(LED2);
    configPinOutput(LED3);
    configPinOutput(LED4);
    configPinInput(SW1);
    configPinInput(SW2);
    configPinInput(SW3);
    // chip 74HC595
    configPinOutput(SDI);
    configPinOutput(SHCLOCK);
    configPinOutput(LTCLOCK);
    configPinInput(POT);
}


// #endif

// #define tempoMorto() delay(15)

// uint8_t leSW1() {
//    volatile static uint8_t BT, BTA, valRetorno;
//    valRetorno = 0;
//    BT = pinRead(SW1);
//    if(BT !=BTA) {
//        if(BTA==1){
//            BTA=0;
//            valRetorno = 1;
//            tempoMorto();
//        }
//        else {
//            BTA=1;
//            tempoMorto();
//        }
//    }
//    return valRetorno
//}

// uint8_t leSW2()  {
//    static uint8_t BT, BTA, valRetorno;
//   valRetorno = 0;
//    BT = pinRead (SW2);
//    if (BT != BTA) {
//        if (BTA==1) {
//            BTA=0;
//            valRetorno = 1;
//            tempoMorto();
//        }
//        else {
//            BTA=1;
//            tempoMorto();
//        }
//    }
//    return valRetorno;
//}


//uint8_t leSW3()  {
//    static uint8_t BT, BTA, valRetorno;
//    valRetorno = 0;
//    BT = pinRead (SW3);
//    if (BT != BTA) {
//        if (BTA==1) {
//            BTA=0;
//            valRetorno = 1;
//            tempoMorto();
//        }
//        else {
//            BTA=1;
//            tempoMorto();
//        }
//    }
//    return valRetorno;
//}

#define tempoMorto() stop=milliss+15

uint8_t leSW1() {
    volatile static uint8_t BT, BTA, valRetorno;
    volatile static uint32_t stop = {0};
    valRetorno = 0;
    if(milliss >= stop) { //tempo morto acabou ?
         BT = pinRead(SW1);
            if(BT != BTA) {
               if(BTA==1){
               BTA=0;
                valRetorno = 1;
             tempoMorto();
             }
            else {
                 BTA=1;
                tempoMorto();
             }
         }
}
    return valRetorno;
}

uint8_t leSW2() {
    volatile static uint8_t BT, BTA, valRetorno;
    volatile static uint32_t stop = {0};
    valRetorno = 0;
    if(milliss >= stop) { //tempo morto acabou ?
         BT = pinRead(SW2);
            if(BT != BTA) {
               if(BTA==1){
               BTA=0;
                valRetorno = 1;
             tempoMorto();
             }
            else {
                 BTA=1;
                tempoMorto();
             }
         }
}
    return valRetorno;
}


uint8_t leSW3() {
    volatile static uint8_t BT, BTA, valRetorno;
    volatile static uint32_t stop = {0};
    valRetorno = 0;
    if(milliss >= stop) { //tempo morto acabou ?
         BT = pinRead(SW3);
            if(BT != BTA) {
               if(BTA==1){
               BTA=0;
                valRetorno = 1;
             tempoMorto();
             }
            else {
                 BTA=1;
                tempoMorto();
             }
         }
}
    return valRetorno;
}

// -----------------------------------------

// Funções de acionamento do módulo de 7 segmentos

volatile uint8_t d7dig1=0, d7dig2=0, d7dig3=0, d7dig4=0;

uint8_t decod7seg[] = { 
    0b00000011, // 0
    0b10011111, // 1
    0b00100101, // 2
    0b00001101, // 3
    0b10011001, // 4
    0b01001001, // 5
    0b01000001, // 6
    0b00011111, // 7
    // separa os bits LSB 
    0b00000001, // 8
    0b00001001, // 9
    0b00010001, // A
    0b11000001, // b
    0b01100011, // C
    0b10000101, // d
    0b01100001, // E
    0b01110001,  // F
    0b11111111 // 16 = display apagado
    };

// -----------------------------------------


void write595(uint8_t displayLigado, uint8_t segmentosLigados) {
    volatile uint16_t temp;

    temp = (displayLigado<<8) | (segmentosLigados); // mesclar os dois bytes em uma word de 16 bits

    for(uint8_t i=0; i<=15; i++) {
        if (temp & 0b1) { pinSet(SDI); }
        else            { pinClear(SDI); }
        pinSet(SHCLOCK); pinClear(SHCLOCK); // pulso de clock do SHIFT CLOCK
        temp = temp >> 1;
    }
    pinSet(LTCLOCK); pinClear(LTCLOCK); // pulso de clock do LATCH CLOCK

}

// -----------------------------------------

void varreduraDisp7() {
    volatile static uint8_t dispAtivo = { 1 };
    volatile static uint8_t contagem;
    contagem--;
    if(contagem==0) {
        //stop = milliss+5;
        contagem = 5;
        switch (dispAtivo) {
            case 1: dispAtivo = 2;
                    write595(0b01000000, decod7seg[d7dig2]);
                    break;
            case 2: dispAtivo = 3;
                    write595(0b00100000, decod7seg[d7dig3]);
                    break;
            case 3: dispAtivo = 4;
                    write595(0b00010000, decod7seg[d7dig4]);
                    break;
            default: dispAtivo = 1;
                    write595(0b10000000, decod7seg[d7dig1]);
                    break;
        }
    }
}

// -----------------------------------------


#endif