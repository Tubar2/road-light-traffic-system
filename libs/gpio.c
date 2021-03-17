/*
 * gpio.c
 *
 *  Created on: Sep 20, 2020
 *      Author: ricardosantos
 */


#include "gpio.h"

/*------------------ Funções Principais ------------------*/
// Funções Flexíveis que serão mais utilzadas pelo usuário
// São capazes de modificar diversas portas e registradores

// Configuração de pinos
void pinConfig(uint8_t port, uint8_t bit, pinConfigs mode)
{
    // Endereço do pino de resistência delcarado de forma global (na função)
    // para que ele seja calculado apenas quando o modo != output
    uint8_t * pinRen;

    // Calcular máscara
    uint8_t mask = 1 << bit;

    // Zera o SEL
    uint8_t * pinSEL = getPin(port, sel);
    clearSinglePinReg(pinSEL, mask);

    // Calcular pino de direção
    uint8_t * pinDir = getPin(port, dir);

    // Calcular pino de saída
    uint8_t * pinOut = getPin(port, out);

    if(mode != output){
        // Calcular pino de Resistência
        pinRen = getPin(port, ren);
    }

    switch (mode) {
    case input:
        setAsInput(mask, pinDir, pinRen, pinOut);
        break;

    case output:
        setAsOutput(mask, pinDir, pinOut);
        break;

    case inPullUp:
        setAsInPUP(mask, pinDir, pinRen, pinOut);
        break;

    case inPullDown:
        setAsInPDwn(mask, pinDir, pinRen, pinOut);
        break;

    default:
        break;
    }
}

void pinConfigPin(Pin * pin, pinConfigs mode){
    // Endereço do pino de resistência delcarado de forma global (na função)
    // para que ele seja calculado apenas quando o modo != output
    uint8_t * pinRen;

    // Calcular máscara
    uint8_t mask = 1 << pin->bit;

    // Zera o SEL
    uint8_t * pinSEL = getPin(pin->port, sel);
    clearSinglePinReg(pinSEL, mask);

    // Calcular pino de direção
    uint8_t * pinDir = getPin(pin->port, dir);

    // Calcular pino de saída
    uint8_t * pinOut = getPin(pin->port, out);

    if(mode != output){
        // Calcular pino de Resistência
        pinRen = getPin(pin->port, ren);
    }

    switch (mode) {
    case input:
        setAsInput(mask, pinDir, pinRen, pinOut);
        break;

    case output:
        setAsOutput(mask, pinDir, pinOut);
        break;

    case inPullUp:
        setAsInPUP(mask, pinDir, pinRen, pinOut);
        break;

    case inPullDown:
        setAsInPDwn(mask, pinDir, pinRen, pinOut);
        break;

    default:
        break;
    }
}

void pinStdConfig(Pin * pin, uint8_t v_sel, uint8_t v_dir, uint8_t v_ren, uint8_t v_out){
    uint8_t * pSEL = getPin(pin->port, sel);
    pinConfigureAddr(pSEL, pin->bit, v_sel);

    uint8_t * pDIR = getPin(pin->port, dir);
    pinConfigureAddr(pDIR, pin->bit, v_dir);

    uint8_t * pREN = getPin(pin->port, ren);
    pinConfigureAddr(pREN, pin->bit, v_ren);

    uint8_t * pOUT = getPin(pin->port, out);
    pinConfigureAddr(pOUT, pin->bit, v_out);

}

void pinFullConfig(Pin * pin, uint8_t v_sel, uint8_t v_dir, uint8_t v_ren, uint8_t v_out, uint8_t v_ie, uint8_t v_ies){
    uint8_t * pSEL = getPin(pin->port, sel);
    pinConfigureAddr(pSEL, pin->bit, v_sel);

    uint8_t * pDIR = getPin(pin->port, dir);
    pinConfigureAddr(pDIR, pin->bit, v_dir);

    uint8_t * pREN = getPin(pin->port, ren);
    pinConfigureAddr(pREN, pin->bit, v_ren);

    uint8_t * pOUT = getPin(pin->port, out);
    pinConfigureAddr(pOUT, pin->bit, v_out);

    uint8_t * pIE = getPin(pin->port, ie);
    pinConfigureAddr(pIE, pin->bit, v_ie);

    uint8_t * pIES = getPin(pin->port, ies);
    pinConfigureAddr(pIES, pin->bit, v_ies);
}

// I/O
uint8_t digitalRead(uint8_t port, uint8_t bit){
    uint8_t mask = 1 << bit;

    uint8_t * pinIn = getPin(port, in);

    return *pinIn & mask;
}

uint8_t digitalReadPin(Pin * pin){
    uint8_t mask = 1 << pin->bit;

    uint8_t * pinIn = getPin(pin->port, in);

    return *pinIn & mask;
}

void digitalWrite(uint8_t port, uint8_t bit, uint8_t value) {
    uint8_t mask = 1 << bit;

    uint8_t * pinOut = getPin(port, out);

    if (value){
        *pinOut |= mask;
    } else {
        *pinOut &= ~mask;
    }
}

void digitalWritePin(Pin * pin, uint8_t value){
    uint8_t mask = 1 << pin->bit;

    uint8_t * pinOut = getPin(pin->port, out);

    if (value){
        *pinOut |= mask;
    } else {
        *pinOut &= ~mask;
    }
}

// Interrupções
void configInterruptibleAddr(uint8_t port, uint8_t bit, edgeSelect edge) {
    uint8_t mask = 1 << bit;

    uint8_t * pinIE = getPin(port, ie);
    uint8_t * pinIES = getPin(port, ies);

    // Habilita a porta como possível geradora de interrupções
    setAsIE(mask, pinIE);

    // Configura sob qual transição de sinal será acionada a interrupção
    setAsIES(mask, pinIES, edge);

    uint8_t * pinIFG = getPin(port, ifg);
    *pinIFG &= ~mask;
}

void configInterruptiblePin(Pin * pin, edgeSelect edge) {
    uint8_t mask = 1 << pin->bit;

    uint8_t * pinIE = getPin(pin->port, ie);
    uint8_t * pinIES = getPin(pin->port, ies);
    uint8_t * pinIFG = getPin(pin->port, ifg);

    // Habilita a porta como possível geardora de interrupções
    setAsIE(mask, pinIE);

    // Configura sob qual transição de sinal será acionada a interrupção
    setAsIES(mask, pinIES, edge);

    // Zera a flag ifg
    *pinIFG &= ~mask;
}

// Endereçamento de portas e registradores
uint8_t* getPin(uint8_t port, pinTypes type){
    uint8_t * pin =
                (uint8_t *) (
                        0x0200                      // base
                        + ((port-1)/2) * 0x20       // base off-set
                        + ((port-1)%2) * 0x01       // IN off-set
                        + type
                );

    return pin;
}

/*------------------ Funções Secundárias ------------------*/
// Funções pouco flexiveis que alteram apenas uma porta de certo registrador
// Essa porta será alterada de forma fixa (set/clear) ou determinada pelo usuário
void setSinglePinReg(uint8_t * pinAddr, uint8_t mask){
    *pinAddr |= mask;
}

void clearSinglePinReg(uint8_t * pinAddr, uint8_t mask){
    *pinAddr &= ~mask;
}

void pinConfigAddr(uint8_t * pinAddr, uint8_t bit, uint8_t val){
    uint8_t mask = val << bit;
    *pinAddr |= mask;
}

void pinMaskConfigAddr(uint8_t * pinAddr, uint8_t mask){
    *pinAddr |= mask;
}

/*------------------ Funções Auxiliares ------------------*/
// Funções que são utilizadas pelas funções de gpio, não devem ser utilizadas
// pelo usuário
void setAsInput(uint8_t mask, uint8_t * pinDir, uint8_t * pinRen, uint8_t * pinOut)
{
    *pinOut &= ~mask;// Clear Px. Out
    *pinDir &= ~mask;// Clear Px. Dir
    *pinRen &= ~mask;// Clear Px. Ren
}

void setAsOutput(uint8_t mask, uint8_t * pinDir, uint8_t * pinOut)
{
    *pinOut &= ~mask;// Clear Px. Out
    *pinDir |= mask; // Set Px. Dir
}

void setAsInPUP(uint8_t mask, uint8_t * pinDir, uint8_t * pinRen, uint8_t * pinOut)
{
    *pinDir &= ~mask; // Clear Px. Dir
    *pinRen |= mask;  // Set Px. Ren
    *pinOut |= mask;  // Set Px. Out
}

void setAsInPDwn(uint8_t mask, uint8_t * pinDir, uint8_t * pinRen, uint8_t * pinOut)
{
    *pinDir &= ~mask; // Clear Px. Dir
    *pinRen |= mask;  // Set Px. Ren
    *pinOut &= ~mask; // Clear Px. Out
}

void setAsIE(uint8_t mask, uint8_t * pinIE){
    *pinIE |= mask;   // Set Px. IE
}

void setAsIES(uint8_t mask, uint8_t * pinIES, uint8_t edge){
    if (edge) {
        *pinIES |= mask;  // Set Px.IES (High to low)
    } else {
        *pinIES &= ~mask; // Clear Px.IES (Low to high)
    }
}

