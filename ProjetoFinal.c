#include <avr/io.h>         // define os registradores
#include <avr/interrupt.h>  // define os vetores de interrupção
#include "lib_arduino.h"    // define bitSet e outras funções de bit
#include "lib_IO_Mega.h"    // mapeamento PORTA-pino
#include "lib_shieldMF.h"  // funções do shield multifunção
#include "lib_Timer2.h"     // funções do timer 2
#include "lib_lcdFinal.h"        // funções do display LCD
#include "lib_adc.h"
#include <stdbool.h>

// Essas definições de pré-processador são usadas para definir os valores de constantes comuns no Arduino, como HIGH, LOW, INPUT e OUTPUT. Também define o tipo bool e seus valores true e false.

typedef bool boolean;
#define HIGH 0x1
#define LOW  0x0
#define INPUT 0x0
#define OUTPUT 0x1

// Definição dos pinos dos LEDs

const int ledPins[] = {10, 11, 12, 13}; // Pinos dos LEDs
boolean modoFestaAtivo = false;

// Variáveis de estado

int currentScreen = 0; // Define uma variável que armazena a tela atual exibida no LCD. Inicia com o valor 0, representando a primeira tela.
int selectedLed = 0; // 0 a 3 para LEDs, 4 para "PRÉ"
int ledIntensities[] = {0, 0, 0, 0}; // Define um array que armazena a intensidade atual de cada LED. Inicia com todas as intensidades em 0.

int lastButton1State = HIGH; // Define variáveis que armazenam o estado anterior de cada botão.
int lastButton2State = HIGH; // Inicia com todos os botões no estado HIGH, indicando que nenhum botão foi pressionado.
int lastButton3State = HIGH; 

// Função para mapear um valor de um intervalo para outro
int map(int x, int in_min, int in_max, int out_min, int out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

ISR(TIMER2_OVF_vect) {
    T2_clearOverflowFlag();   // clear bit timer 2 overflow flag (TOV2)
    pinSet(LCDVP);
    pinSet(TESTPIN);
    milliss++;   // incremento do millis
    varreduraDisp7();
    // if( contTempoMorto>0 ) contTempoMorto--;
}

ISR(TIMER2_COMPA_vect) {
    bitSet(TIFR2,OCF2A);  // zerar a FLAG que sinaliza o evento compare match A
    pinClear(LCDVP);
    pinClear(TESTPIN);
}

// --------------------------------------------------------------------------------

// COMEÇO DO CÓDIGO --------------------

// Função para gerar número pseudo-aleatório
    unsigned int meuRandom(unsigned int min, unsigned int max) {
    static unsigned int seed = 0; // Seed inicial estática
    unsigned long millisTime = millis(); // Obtém o tempo atual em milissegundos

    seed = (seed * millisTime + 3) % 32749; // Algoritmo usando millis() para mudar a seed
    return min + (seed % (max - min + 1));
}

void controlarIntensidadeLED(int pinLED, int intensidade) {
    // Verifica se a intensidade está no intervalo válido (0 a 255)
    if (intensidade < 0) intensidade = 0;
    if (intensidade > 255) intensidade = 255;

    // Calcula o tempo de delay proporcional à intensidade
    int tempoLigado = map(intensidade, 0, 255, 0, 5000); // Converter um valor de intensidade (que pode variar de 0 a 255) em um intervalo de tempo (de 0 a 5000 microssegundos, ou 5 milissegundos). Ajustar conforme necessário.

    // Quando intensidade é 0, map(intensidade, 0, 255, 0, 5000) resulta em 0. Isso significa que o LED ficará totalmente desligado (tempo de ligação zero).

    // Quando intensidade é 255, map(intensidade, 0, 255, 0, 5000) resulta em 5000. Neste caso, o LED ficará totalmente ligado (tempo de ligação máximo de 5000 microssegundos).

    // Valores intermediários de intensidade resultarão em valores de tempoLigado proporcionais, criando assim um efeito de PWM por software, onde o LED é ligado e desligado rapidamente, variando a média de tempo em que ele está ligado para simular diferentes níveis de intensidade luminosa.

    // Ajustar o intervalo de saída (0, 5000) conforme necessário para controlar a suavidade do efeito de PWM ou para ajustar a resposta do LED à intensidade desejada. Por exemplo, aumentar o valor máximo (5000) pode fazer com que o LED pisque menos perceptivelmente em intensidades baixas.

    // Liga o LED pelo tempo proporcional à intensidade
    pinSet(pinLED);
    delayMicroseconds(tempoLigado);

    // Desliga o LED pelo tempo restante para completar o ciclo
    pinClear(pinLED);
    delayMicroseconds(5000 - tempoLigado); // Ajustar conforme necessário
}

int lerIntensidadeLED(int pinLED) {
    // Lê a intensidade atual do LED
    int valorADC = pinRead(POT);
    int intensidade = map(valorADC, 0, 1023, 0, 255); // Mapeia o valor lido para uma escala de 0 a 255
    return intensidade;
}


void modoLeitura() {    
    for (int i = 0; i < 4; i++) {
        controlarIntensidadeLED(ledPins[i], 128);
    }
}

void modoDescanso() {
    for (int i = 0; i < 4; i++) {
        controlarIntensidadeLED(ledPins[i], 0);
    }
}

void piscarAleatorio(int pinLED) {
    int tempoligado = meuRandom(1000, 4000); // Gera um tempo aleatório entre 1000 e 4000 microssegundos (1 a 4 milissegundos)
    pinSet(pinLED);
    delayMicroseconds(tempoligado);
    pinClear(pinLED);
    delayMicroseconds(5000 - tempoligado); // Garante que o ciclo completo seja de 5000 microssegundos
}

void modoFesta() {
    modoFestaAtivo = true; // Ativa o modo festa

    while (modoFestaAtivo) {
        for (int i = 0; i < 4; i++) {
            piscarAleatorio(ledPins[i]); // Piscar cada LED aleatoriamente
        }
        
        if(leSW3()==1) {
            modoFestaAtivo = false; // Desativa o modo festa ao pressionar o botão
        } 
    }

    // Ao sair do modo festa, limpar os LEDs
    for (int i = 0; i < 4; i++) {
        pinClear(ledPins[i]);
    }
}

// lcdComando(0x80); // Posiciona o cursor na primeira linha, coluna 0
// lcdComando(0xC0); // 0xC0 é o comando para a segunda linha no LCD 16x2

void mostrarTelaInicial() {
    lcdClear(); // Limpa o display LCD
    lcdString("LEDs:1 2 3 4 PRE\n");
    lcdCursorOff();

    // Posiciona o cursor na segunda linha para os indicadores
    lcdComando(0xC0); // 0xC0 é o comando para a segunda linha no LCD 16x2

    for (int i = 0; i < 5; i++) {
        if (i == selectedLed) {
            lcdString("^"); // Indicador de seleção
        } else {
            lcdString(" ");
        }

        // Adiciona espaços entre os indicadores para alinhar corretamente
        if (i < 4) {
            lcdString(" ");
        } else {
            lcdString("  "); // Espaço extra após o último indicador
        }
    }
}

void mostrarTelaLED1() {
    int intensidade = lerIntensidadeLED(ledPins[currentScreen - 1]);
    lcdClear();
    lcdString("      LED1      \n");
    lcdString("Voltagem = "); // Adiciona a string "Voltagem = "
    lcdComando(0xCB); // Posiciona o cursor após "Voltagem = "
    char buffer[5]; // Tamanho do buffer ajustado conforme necessário
    intToStr(intensidade, buffer, sizeof(buffer)); // Converte intensidade para string
    lcdString(buffer); // Exibe a intensidade convertida
    lcdCursorOff();
}

void mostrarTelaLED2() {
    int intensidade = lerIntensidadeLED(ledPins[currentScreen - 1]);
    lcdClear();
    lcdString("      LED2      \n");
    lcdString("Voltagem = "); // Adiciona a string "Voltagem = "
    lcdComando(0xCB); // Posiciona o cursor após "Voltagem = "
    char buffer[5]; // Tamanho do buffer ajustado conforme necessário
    intToStr(intensidade, buffer, sizeof(buffer)); // Converte intensidade para string
    lcdString(buffer); // Exibe a intensidade convertida
    lcdCursorOff();
}

void mostrarTelaLED3() {
    int intensidade = lerIntensidadeLED(ledPins[currentScreen - 1]);
    lcdClear();
    lcdString("      LED3      \n");
    lcdString("Voltagem = "); // Adiciona a string "Voltagem = "
    lcdComando(0xCB); // Posiciona o cursor após "Voltagem = "
    char buffer[5]; // Tamanho do buffer ajustado conforme necessário
    intToStr(intensidade, buffer, sizeof(buffer)); // Converte intensidade para string
    lcdString(buffer); // Exibe a intensidade convertida
    lcdCursorOff();
}

void mostrarTelaLED4() {
    int intensidade = lerIntensidadeLED(ledPins[currentScreen - 1]);
    lcdClear();
    lcdString("      LED4      \n");
    lcdString("Voltagem = "); // Adiciona a string "Voltagem = "
    lcdComando(0xCB); // Posiciona o cursor após "Voltagem = "
    char buffer[5]; // Tamanho do buffer ajustado conforme necessário
    intToStr(intensidade, buffer, sizeof(buffer)); // Converte intensidade para string
    lcdString(buffer); // Exibe a intensidade convertida
    lcdCursorOff();
}

void mostrarTelaPreDefinicoes() {
    lcdClear(); // Limpa o display LCD
    lcdString("\fLei Des Fes\n"); // Exibe "Leitura", "Descanso" e "Festa" na primeira linha
    lcdCursorOff();

    // Tamanho dos espaços entre as opções
    int espacoEntreOpcoes = 3;

    for (int i = 0; i < 3; i++) {
        if (i == selectedLed) {
            lcdString("^"); // Indicador de seleção
        } else {
            lcdString(" ");
        }

        // Adiciona espaços entre os indicadores para alinhar corretamente
        if (i < 2) {
            for (int j = 0; j < espacoEntreOpcoes; j++) {
                lcdString(" ");
            }
        }
    }
}

void setup() {
    cli();   // disable interrupts
    setupShieldMF();
    setupTimer2();
    T2_enableInterruptOV();   // bitSet(TIMSK2, TOIE2)
    T2_clearOverflowFlag();   // bitSet(TIFR2, TOV2)
    T2_interruptEnableCMPA(); // bitSet(TIMSK2, OCIE2A)
    sei();  // enable interrupts
    setupLCD8();
    initLCD8();
    mostrarTelaInicial(); // Exibe a tela inicial

    // Configurar LEDs como saída
    for (int i = 0; i < 4; i++) {
        configPinOutput(ledPins[i]);
    }
}

void loop() {
    byte button1State = pinRead(SW1);
    byte button2State = pinRead(SW2);
    byte button3State = pinRead(SW3);



    if (currentScreen == 0) {
        if (button1State == LOW && lastButton1State == HIGH) {
            selectedLed = (selectedLed + 1) % 5;
            mostrarTelaInicial();
        }
        if (button2State == LOW && lastButton2State == HIGH) {
            if (selectedLed < 4) {
                currentScreen = selectedLed; // Mapeamento para telas dos LEDs
                switch (selectedLed) {
                    case 0:
                        mostrarTelaLED1();
                        break;
                    case 1:
                        mostrarTelaLED2();
                        break;
                    case 2:
                        mostrarTelaLED3();
                        break;
                    case 3:
                        mostrarTelaLED4();
                        break;
                }
            } else {
                currentScreen = 5; // Modo pré-definido
                mostrarTelaPreDefinicoes();
            }
        }
    } else if (currentScreen >= 1 && currentScreen <= 4) { // Telas dos LEDs

        int intensidade = lerIntensidadeLED(ledPins[currentScreen - 1]);

        if (button1State == LOW && lastButton1State == HIGH) {
            // Aumentar intensidade
            intensidade += 10; // Ajuste conforme necessário
            controlarIntensidadeLED(ledPins[currentScreen - 1], intensidade);
            mostrarTelaLED(currentScreen);
        }
        if (button2State == LOW && lastButton2State == HIGH) {
            // Diminuir intensidade
            intensidade -= 10; // Ajuste conforme necessário
            controlarIntensidadeLED(ledPins[currentScreen - 1], intensidade);
            mostrarTelaLED(currentScreen);
        }
        if (currentScreen >= 1 && currentScreen <= 4) { // Telas dos LEDs
        if (button3State == LOW && lastButton3State == HIGH) {
            // Voltar para a tela inicial
            currentScreen = 0;
            mostrarTelaInicial();
        }
    } else if (currentScreen == 5) { // Tela de modos pré-definidos
        if (button1State == LOW && lastButton1State == HIGH) {
            selectedLed = (selectedLed + 1) % 3;
            mostrarTelaPreDefinicoes();
        }
        if (button2State == LOW && lastButton2State == HIGH) {
            if (selectedLed == 0) {
                modoLeitura();
            } else if (selectedLed == 1) {
                modoDescanso();
            } else if (selectedLed == 2) {
                modoFesta();
            }
            currentScreen = 0;
            mostrarTelaInicial();
        }
        if (button3State == LOW && lastButton3State == HIGH) {
            currentScreen = 0;
            mostrarTelaInicial();
        }
    }

    lastButton1State = button1State;
    lastButton2State = button2State;
    lastButton3State = button3State;
}
}