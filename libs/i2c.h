#ifndef I2C_H_
#define I2C_H_

#include <msp430.h>
#include <stdint.h>
#include <stdlib.h>


void    i2cInitEscravo_0(uint8_t addr);
void    i2cInitMestre_1(uint8_t addr);

uint8_t i2cSend(uint8_t addr, uint8_t * data, uint8_t nBytes);
uint8_t i2cSendByte(uint8_t addr, uint8_t data);

uint8_t i2cGet(uint8_t addr, uint8_t * data, uint8_t nBytes);
uint8_t i2cGetByte(uint8_t addr);

uint8_t* i2cAllAdresses();


#endif /* I2C_H_ */
