/*
 * projFinal.c
 */


#include "projFinal.h"

/*------------------ Funções Principais ------------------*/

void switchSemState(Semaforo * cur, Semaforo * prox){
    wait(3, sec, TA_0);
    // Apaga verde, acende amarelo do semaforo atual
    digitalWritePin(&cur->g, 0);
    digitalWritePin(&cur->y, 1);
    // Aguarda o tempo decidido
    wait(LIGHT_TIME, TA_0);
    // Apaga amarelo, acende vermelho do semaforo atual
    digitalWritePin(&cur->y, 0);
    digitalWritePin(&cur->r, 1);
    // Aguarda o tempo decidido
    wait(LIGHT_TIME, TA_0);
    // Apaga vermelho, acende verde do prox semaforo
    digitalWritePin(&prox->r, 0);
    digitalWritePin(&prox->g, 1);

}

void initSemState(Semaforo * s1, Semaforo * s2){
    // Comeca com s1 no verde e s2 no vermelho
    digitalWritePin(&s1->g, 1);
    digitalWritePin(&s2->r, 1);
}

void configSem(Semaforo * sem){
    // Configura todos os pinos (vermelho, verde e amarelo) do semaforo como saida
    pinConfigPin(&sem->r, output);
    pinConfigPin(&sem->g, output);
    pinConfigPin(&sem->y, output);

}

void printUARTnum(uint8_t num){
    uint8_t unidade, dezena, centena, resto;
    centena = num/100;
    resto = num%100;

    dezena = resto/10;
    resto = resto%10;

    unidade = resto;

    uartSend(centena + 0x30);
    uartSend(dezena + 0x30);
    uartSend(unidade + 0x30);
}

// Interruption dos timers
#pragma vector = TIMER1_A0_VECTOR
__interrupt void TA1_ISR(){
    tMin_Counter++;
    // Aguarda 15 segundos de tempo minimo
    if (tMin_Counter == 10){
        tMin_Counter = 0;
        tMin_flag = 1;
        P1OUT |= BIT0;              // Liga o LED vermelho quando passa o tempo minimo
        TA1CTL = MC__STOP | TACLR;  // Para o timer pra comecar do zero
    }
}

// Interruptions dos lasers
#pragma vector = PORT2_VECTOR
__interrupt void P2_ISR(){
    switch(P2IV) {
    case 2:     // Interruption no 2.0
        // Habilita a flag de carro presente no semaforo 1
        l1_flag = 1;

        break;
    case 4:     // Interruption no 2.1
        endSimulation = 1;

        P2IE &= ~BIT1;
        TA2CTL |= MC__UP | TACLR;

        P2IES ^= BIT1;

         break;
    case 6:     // Interruption no 2.2
        // Habilita a flag de carro presente no semaforo 2
        l2_flag = 1;

        break;
    default:
        break;
    }
}

// Function de interruption no timer para controlar o debounce no switch
#pragma vector = TIMER2_A0_VECTOR
__interrupt void TA1_CCR0_ISR(){
    P2IE |= BIT1;

    TA2CTL &= ~MC__UP;

    P2IFG &= ~BIT1;
}

