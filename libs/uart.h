#ifndef _UART_H
#define _UART_H

#include <stdint.h>
#include <msp430.h>

void uartInit();

void uartPrint(char * str);

void uartSend(uint8_t byte);
uint8_t uartGet(char * data);

#endif
