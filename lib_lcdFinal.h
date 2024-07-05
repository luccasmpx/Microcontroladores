// ------------------------------------------------------
// Biblioteca do display LCD alfanumérico
// ------------------------------------------------------

#ifndef lib_lcdFinal_h
#define lib_lcdFinal_h

#include <avr/io.h>         // define os registradores
#include "lib_arduino.h"    // define bitSet e outras funções de bit
#include "lib_IO_Mega.h"    // define mapeamento dos pinos
// #include <Arduino.h>

// Definição dos pinos do display LCD
#define LCDVCC 53
#define LCDVP  51
#define LCDRS  49   // RS: 0=comando, 1=dados
#define LCDRW  47
#define LCDEN  45   // EN: pulso de clock 0 -> 1 -> 0
#define LCD0   43   // D0 (não usado no barramento de 4 bits)
#define LCD1   41   // D1 (não usado no barramento de 4 bits)
#define LCD2   39   // D2 (não usado no barramento de 4 bits)
#define LCD3   37   // D3 (não usado no barramento de 4 bits)
#define LCD4   35   // D4 / D0 (no barramento de 4 bits)
#define LCD5   33   // D5 / D1 (no barramento de 4 bits)
#define LCD6   31   // D6 / D2 (no barramento de 4 bits)
#define LCD7   29   // D7 / D3 (no barramento de 4 bits)
#define TESTPIN 5   // pino usado para teste (verificação de sinal PWM)

#define lcdComando(x) lcdWrite8(0, x)
#define lcdDado(x) lcdWrite8(1, x)
#define lcdClear() lcdWrite8(0, 0b00000001); delay(3)
#define lcdCursorOff()   lcdComando(0b00001100)
#define lcdCursorOn()    lcdComando(0b00001110)
#define lcdCursorBlink() lcdComando(0b00001111)

// Protótipos das funções
void setupLCD4();
void lcdWrite4(uint8_t RSmode, uint8_t ch);
void initLCD4();

void setupLCD8();
void lcdWrite8(uint8_t RSmode, uint8_t ch);
void initLCD8();
void lcdString(const char *ch);
void intToStr(int num, char* buffer, int bufferSize);

void intToStr(int num, char* buffer, int bufferSize) {
    int i = 0;
    int tempNum = num;
    int numLength = 0;

    // Calcula o número de dígitos
    while (tempNum) {
        numLength++;
        tempNum /= 10;
    }

    // Adiciona o caractere nulo de término da string
    buffer[numLength] = '\0';

    // Preenche o buffer com os dígitos do número
    for (i = numLength - 1; i >= 0; i--) {
        buffer[i] = num % 10 + '0'; // Converte o dígito para um caractere ASCII
        num /= 10;
    }
}

// Função para configurar os pinos do LCD para o modo 4 bits
void setupLCD4() {
    configPinOutput(LCD4);
    configPinOutput(LCD5);
    configPinOutput(LCD6);
    configPinOutput(LCD7);
    configPinOutput(LCDRS);
    configPinOutput(LCDRW);
    configPinOutput(LCDEN);
    configPinOutput(LCDVP);
    configPinOutput(LCDVCC);
    pinSet(LCDVCC);  // Alimentação 5V
    pinClear(LCDVP); // contraste máximo
    pinClear(LCDRS); // RS=0 inicialmente
    pinClear(LCDRW); // coloca em modo de escrita (Write)
    pinClear(LCDEN); // condição inicial do CLOCK
}

void lcdWrite4(uint8_t RSmode, uint8_t ch) {
    // Envia os 4 bits mais significativos
    if (RSmode) pinSet(LCDRS); else pinClear(LCDRS);
    if (bitRead(ch, 7)) pinSet(LCD7); else pinClear(LCD7);
    if (bitRead(ch, 6)) pinSet(LCD6); else pinClear(LCD6);
    if (bitRead(ch, 5)) pinSet(LCD5); else pinClear(LCD5);
    if (bitRead(ch, 4)) pinSet(LCD4); else pinClear(LCD4);
    pinSet(LCDEN);  // Pulso do enable
    delayMicroseconds(2);
    pinClear(LCDEN);
    delayMicroseconds(100);

    // Envia os 4 bits menos significativos
    if (bitRead(ch, 3)) pinSet(LCD7); else pinClear(LCD7);
    if (bitRead(ch, 2)) pinSet(LCD6); else pinClear(LCD6);
    if (bitRead(ch, 1)) pinSet(LCD5); else pinClear(LCD5);
    if (bitRead(ch, 0)) pinSet(LCD4); else pinClear(LCD4);
    pinSet(LCDEN);  // Pulso do enable
    delayMicroseconds(2);
    pinClear(LCDEN);
    delayMicroseconds(100);
}

void initLCD4() {
    delayMicroseconds(50000); // tempo de espera após ligar o kit
    lcdWrite4(0, 0b0010); // Inicializa em modo 4 bits
    lcdWrite4(0, 0b0010); // Repetição para garantir o modo 4 bits
    lcdWrite4(0, 0b1000); // 4 bits, 2 linhas, matriz 5x7

    lcdWrite4(0, 0b00001111); // display ON, cursor ON, piscante
    lcdWrite4(0, 0b00000001); // display clear (3 ms)
    delay(3);
    lcdWrite4(0, 0b00000110); // entry mode set
}

// Função para configurar os pinos do LCD para o modo 8 bits
void setupLCD8() {
    configPinOutput(LCD0);
    configPinOutput(LCD1);
    configPinOutput(LCD2);
    configPinOutput(LCD3);
    configPinOutput(LCD4);
    configPinOutput(LCD5);
    configPinOutput(LCD6);
    configPinOutput(LCD7);
    configPinOutput(LCDRS);
    configPinOutput(LCDRW);
    configPinOutput(LCDEN);
    configPinOutput(LCDVP);
    configPinOutput(LCDVCC);
    pinSet(LCDVCC);  // Alimentação 5V
    pinClear(LCDVP); // contraste máximo
    pinClear(LCDRS); // RS=0 inicialmente
    pinClear(LCDRW); // coloca em modo de escrita (Write)
    pinClear(LCDEN); // condição inicial do CLOCK
    pinClear(LCD0);  // alguns programadores recomendam...
    pinClear(LCD1);  // deixar esses sinais aterrados
    pinClear(LCD2);
    pinClear(LCD3);
    // T2_interruptEnableCMPA(); // necessário para PWM
    OCR2A = 100; // valor PWM de VP calibrado na função ajustPwmLcd do arquivo lcd.c
    configPinOutput(TESTPIN);
}

// Função para escrever um byte no LCD
void lcdWrite8(uint8_t RSmode, uint8_t ch) {
    if(RSmode) pinSet(LCDRS); else pinClear(LCDRS); // prepara o sinal RS
    if(bitRead(ch, 7)) pinSet(LCD7); else pinClear(LCD7);
    if(bitRead(ch, 6)) pinSet(LCD6); else pinClear(LCD6);
    if(bitRead(ch, 5)) pinSet(LCD5); else pinClear(LCD5);
    if(bitRead(ch, 4)) pinSet(LCD4); else pinClear(LCD4);
    if(bitRead(ch, 3)) pinSet(LCD3); else pinClear(LCD3);
    if(bitRead(ch, 2)) pinSet(LCD2); else pinClear(LCD2);
    if(bitRead(ch, 1)) pinSet(LCD1); else pinClear(LCD1);
    if(bitRead(ch, 0)) pinSet(LCD0); else pinClear(LCD0);
    pinSet(LCDEN);
    delayMicroseconds(2);
    pinClear(LCDEN);
    delayMicroseconds(100); // delayMicroseconds(50);
}

// Função para inicializar o LCD com barramento de 8 bits
void initLCD8() {
    delayMicroseconds(50000); // tempo de espera após ligar o kit
    delayMicroseconds(30000); // tempo de espera após ligar o kit

    lcdWrite8(0, 0b00111000); // 8 bits, tentativa 1
    delayMicroseconds(4500);
    lcdWrite8(0, 0b00111000); // 8 bits, tentativa 2
    delayMicroseconds(4500);
    lcdWrite8(0, 0b00111000); // barramento 8 bits, 2 linhas, matriz 5x7
    delayMicroseconds(4500);

    lcdWrite8(0, 0b00001111); // display ON, cursor ON, piscante
    lcdWrite8(0, 0b00000001); // display clear (3 ms)
    delay(3);
    lcdWrite8(0, 0b00000110); // entry mode set
}

// Função para escrever uma string no LCD
void lcdString(const char *ch) {
    while (*ch != 0) {
        if (*ch == '\n') {
            lcdComando(0xC0); // Nova linha no LCD
        } else if (*ch == '\f') {
            lcdClear(); // Limpar o display do LCD
        } else {
            lcdDado(*ch); // Escrever caractere no LCD
        }
        ch++;
    }
}

#endif