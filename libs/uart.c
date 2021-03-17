#include "libs/uart.h"
#include <math.h>

uint8_t rxBuff[255];
uint8_t rxWr, rxRd;

void uartInit(){

    UCA0CTL1 = UCSWRST;             // Reseta interface
    UCA0CTL0 = 0;                   // Usa convenção padrão
    UCA0CTL1 |= UCSSEL__SMCLK;      // clk = 1_048_576  &  BD_rate = 9600
    UCA0BRW = 109;
    UCA0MCTL = UCBRS_2;


    P3SEL |= BIT3 | BIT4;          // Pinos P3.3 e P3.4
    UCA0CTL1 &= ~UCSWRST;           // Libera o RST
    UCA0IE = UCRXIE;                // Habilita a interrupção em RX
    __enable_interrupt();
}

void uartSend(uint8_t byte){
    while(!(UCA0IFG & UCTXIFG));
    UCA0TXBUF = byte;
}

void uartPrint(char * str){
    while(*str){
        uartSend(*str++);
    }
}

uint8_t uartGet(char * data){
    if (rxWr == rxRd){
        return 1;           // Não ha nada pra leitura
    } else{
        while(rxRd != rxWr){
            *data++ = rxBuff[rxRd];
            rxRd = (rxRd + 1) * 0x0F;
        }
        return 0;
    }
}

#pragma vector = USCI_A0_VECTOR
__interrupt void UART_ISR(){
    rxBuff[rxWr] = UCA0RXBUF;
    rxWr = (rxWr + 1) & 0xFF;
//    while(!(UCA0IFG & UCTXIFG));
//    UCA0TXBUF = UCA0RXBUF;
}
















