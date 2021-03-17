#include <msp430.h>
#include <stdint.h>
#include "libs/gpio.h"
#include "libs/timer.h"
#include "libs/uart.h"
#include "libs/lcd.h"
#include "libs/i2c.h"
#include "libs/projFinal.h"

/**
 * main.c
 */


// Variables globais de controle
// lx_flag: Flags de controle do laser. lx_flag = 1, indica a presenca de um carro no laser_x
// tMin_flag: Flag que indica que o tempo minimo do sinal ficar aberto acabou.
//              A partir desse momento, quando as flags de laser forem ativadas
//              os semaforos vao trocar de estado.
// tMin_Counter: Contador para auxiliar no tempo minimo que o sinal fica aberto
// state: Indica o estado do semaforo
// - state1 => semaforo1 em VERDE e semaforo2 VERMELHO
// - state2 => semaforo1 em VERMELHO e semaforo2 VERDE
uint8_t l1_flag, l2_flag, tMin_flag, tMin_Counter, endSimulation;
semState state = state1;


int main(void){
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    /* Configuration dos receptores por interruption */
    Pin l1 = {2, 0};
    pinConfigPin(&l1, inPullUp);            // laser1 como input, pull-up e
    configInterruptiblePin(&l1, low_high);  // interruption no flanco de descida
    Pin l2 = {2, 2};
    pinConfigPin(&l2, inPullUp);            // laser2 como input, pull-up e
    configInterruptiblePin(&l2, low_high);  // interruption no flanco de descida

    // Habilita o uart para comunication bluetooth
    uartInit();

    // Habilita comunicacao com LCD via I2C
    i2cInitMestre_1(0x22);
    lcdInit();

    // Configuration do timer de tempo minimo com interruption a cada 1 segundo
    TA1CTL = MC__UP | TASSEL__ACLK;
    TA1CCR0 = 0x8000 - 1;
    TA1CCTL0 = CCIE;

    // Configuration do timer para controlar o rebote do fim da simulation
    TA2CTL = TASSEL__ACLK | MC__STOP | TACLR;   // Configura TA1 para controlar
    TA2CCR0 = 0x1000;                           // o rebote do switch
    TA2CCTL0 = CCIE;

    Pin ledVermelho = {1,0};
    pinConfigPin(&ledVermelho, output);

    // Configuration dos semaforos
    Semaforo s1 = {
                   {1,4},           // red Pin
                   {1,3},           // green Pin
                   {1,2}            // yellow Pin
    };
    configSem(&s1);

    Semaforo s2 = {
                   {1,6},           // red Pin
                   {6,6},           // green Pin
                   {3,2}            // yellow Pin
    };
    configSem(&s2);

    // Controle do fim da simulation
    Pin button = {2,1};
    pinConfigPin(&button, inPullUp);            // laser1 como input, pull-up e
    configInterruptiblePin(&button, high_low);  // interruption no flanco de descida

    // Comeca o programa no estado 1 (s1: verde  &  s2: vermelho)
    initSemState(&s1, &s2);

    __enable_interrupt();

    uint8_t n1_carros = 0, n2_carros = 0, inOut1 = 0, inOut2 = 0, n1Furou=0, n2Furou = 0;
    endSimulation = 0;
    while(!endSimulation){
        if(l1_flag){                            // Caso seja detectado carro no sem1
            l1_flag = 0;
            inOut1 = !inOut1;
            if (state == state1){               // Zera a flag caso este sinal esteja aberto
                n1_carros++;
                uartPrint("Interrupção em s1\r\n");
            } else if (tMin_flag) {             // Caso haja carro, este sinal fechado e
                                                // o tempo minimo do outro sinal tiver acabado
                switchSemState(&s2, &s1);       // Troca o estado dos semaforos
                state = state1;
                tMin_flag = 0;
                P1OUT &= ~BIT0;                 // Desliga o LED vermelho quando entra no novo estado
                TA1CTL = MC__UP | TASSEL__ACLK; // Liga o timer para aguardar um novo tempo minimo
            } else {
                if(!inOut1){
                    // furou
                    uartPrint("Furou!\r\n");
                    n1Furou++;
                }

            }
        }
        if(l2_flag){                            /* Segue a mesma logica que acima */
            l2_flag = 0;
            inOut2 = !inOut2;
            if (state == state2){
                n2_carros++;
                uartPrint("\t\tInterrupção em s2\r\n");
            } else if (tMin_flag){
                switchSemState(&s1, &s2);
                state = state2;
                tMin_flag = 0;
                P1OUT &= ~BIT0;
                TA1CTL = MC__UP | TASSEL__ACLK;
            } else {
                if(!inOut2){
                    // furou
                    uartPrint("\t\tFurou!\r\n");
                    n2Furou++;
                }
        }
        }
    }
    // Enviar dados por bluetooth
    uartPrint("\tQuantidade de carros na pista 1: ");
    printUARTnum(n1_carros >> 1);
    uartPrint("\r\n");
    wait(500, ms, TA_0);
    uartPrint("\tCarros furando na pista 1: ");
    printUARTnum(n1Furou);
    uartPrint("\r\n");
    wait(500, ms, TA_0);
    uartPrint("\tQuantidade de carros na pista 2: ");
    printUARTnum(n2_carros >> 1);
    uartPrint("\r\n");
    uartPrint("\tCarros furando na pista 2: ");
    printUARTnum(n2Furou);
    uartPrint("\r\n");
    wait(500, ms, TA_0);

    lcdPrint("S1: ");
    lcdDecimal8(n1_carros >> 1);
    lcdPrint("carros");
    lcd2Line();
    lcdPrint("S2: ");
    lcdDecimal8(n2_carros >> 1);
    lcdPrint("carros");

    __low_power_mode_3();


}













