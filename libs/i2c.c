#include "i2c.h"
#include "gpio.h"
#include "timer.h"

uint8_t echo = 0;

void i2cInitEscravo_0(uint8_t addr){

    // UCB0 -> Escravo
    UCB0CTLW0 = UCSWRST;                            // Seta o RST
    UCB0CTLW0 |= 0x0700;
    UCB0I2COA = addr;

    // Conectar SDA - P3.0 e SCL - P3.1
    P3SEL |= (BIT0 | BIT1);                         // Usa os pinos P3.{0,1}
    pinConfig(3, 0, inPullUp);
    pinConfig(3, 1, inPullUp);

    UCB0CTL1 &= ~UCSWRST;                           // Libera o RST
    UCB0IE = UCRXIE | UCTXIE;
}

void i2cInitMestre_1(uint8_t addr){

    // UCB0 -> Mestre
    UCB0CTLW0 = UCSWRST;                            // Seta o RST
    UCB0CTLW0 |= 0x0F00 | UCSSEL__SMCLK;            // Escolhe o SMCLK
    UCB0BRW = 100;                                  // Frequ�ncia dividida do clock
    UCB0I2COA = addr;

    // Conectar SDA - P3.0 e SCL - P3.1
    pinConfig(3, 0, inPullUp);
    pinConfig(3, 1, inPullUp);                         // Usa os pinos P4.{1,2}
    P3SEL |= (BIT0 | BIT1);                         // Usa os pinos P3.{0,1}

    UCB0CTL1 &= ~UCSWRST;                           // Libera o RST
}

uint8_t i2cSend(uint8_t addr, uint8_t * data, uint8_t nBytes){

    UCB0I2CSA = addr;                               // Configura endere�o
    UCB0CTLW0 |= UCTXSTT | UCTR;                    // Transmite endere�o

    while(!(UCB0IFG & UCTXIFG));                    // Espera start acontecer
    UCB0TXBUF = *data++;                            // Escreve o primeiro byte para
    nBytes--;                                       // Destravar o ciclo de ACK

    while(UCB0CTLW0 & UCTXSTT);                     // Aguarda ACK/NACK

    if(UCB0IFG & UCNACKIFG){
        UCB0CTLW0 |= UCTXSTP;                       // Se o escravo n�o respondeu
        while(UCB0CTLW0 & UCTXSTP);                 // eu pe�o um stop e espero o stop
        return 1;                                   // ser enviado antes de retornar
    }

    while(nBytes--){                                // Se foi um ACK, ent�o continuamos
        while(!(UCB0IFG & UCTXIFG));                // c/ a comunica��o enviando os pr�ximos
        UCB0TXBUF = *data++;                        // bytes toda vez que TXBUF ficar vazio
    }

    while(!(UCB0IFG & UCTXIFG));                    // Espera enviar o �ltimo byte
    UCB0CTLW0 |= UCTXSTP;                           // e pedir um stop

    while(UCB0CTLW0 & UCTXSTP);                     // Espera o stop ser enviado

    return 0;
}

uint8_t i2cSendByte(uint8_t addr, uint8_t data){

    return i2cSend(addr, &data, 1);
}

uint8_t i2cGetByte(uint8_t addr){

    uint8_t byte = 0;
    i2cGet(addr, &byte, 1);
    return byte;
}


uint8_t i2cGet(uint8_t addr, uint8_t * data, uint8_t nBytes){

    __enable_interrupt();

    UCB0IFG = 0;

    UCB0I2CSA = addr;                               // Endere�o do escravo
    UCB0CTLW0 &= ~UCTR;                             // Pede start como receptor
    UCB0CTLW0 |= UCTXSTT;

    while(UCB0CTLW0 & UCTXSTT);                     // Espera ciclo de ACK

    if (UCB0IFG & UCNACKIFG){                       // Se for um NACK
        UCB0CTLW0 |= UCTXSTP;                       // Pede stop e para
        while(UCB0CTLW0 & UCTXSTP);
        return 1;
    }

    while(--nBytes){
        while(!(UCB0IFG & UCRXIFG));                // Recebe n-1 bytes
        *data++ = UCB0RXBUF;
    }

    UCB0CTLW0 |= UCTXSTP;                           // Antes do �ltimo, pede stop

    while(UCB0CTLW0 & UCTXSTP);                     // Aguarda envio do Stop

    return 0;
}

uint8_t* i2cAllAdresses(){

    uint8_t i = 8, count = 0;
    uint8_t *vector = calloc(120, sizeof(uint8_t));

    for (i = 8; i <= 128; i++){
        if (!i2cSendByte(i, 0xF)){
            vector[count] = i;
            count++;
        }
    }

    uint8_t *retorno = calloc(count, sizeof(uint8_t));

    for (i = 0; i < count; i++){
        retorno[i] = vector[i];
    }

    return retorno;
}

/*
#pragma vector = USCI_B0_VECTOR
__interrupt void I2C_ISR_Escravo(){
    switch (UCB0IV){
        case USCI_I2C_UCRXIFG:
            echo = UCB0RXBUF;
            break;

        case USCI_I2C_UCTXIFG:
            UCB0TXBUF = echo;
            break;

        default: break;
    }
}
*/


