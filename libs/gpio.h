/*
 * gpio.h
 *
 *  Created on: Sep 20, 2020
 *      Author: ricardosantos
 */

/*
 * A configuração dos pinos pode ser encontrada no documeno
 * UserGuide-rev da texas; capítulo 12: Digital I/O Module
 * A seção 12.2 possui especificaões dos registradores e portas
 * A tabela em 12.4 detalha os offsets utilizados na criação dos pinos
 * */

#include <stdint.h>

#ifndef LIBS_GPIO_H_
#define LIBS_GPIO_H_

/* Definição de Tipos */

// enumerador que define tipos de configuração básica para os pinos
// input: OUT=0, DIR=0, Ren=0
// output: OUT=0, DIR=1
// inPullUp: DIR=0, REN=1, OUT=1
// inPullDown: DIR=0, REN=1, OUT=0
typedef enum { input, output, inPullUp, inPullDown } pinConfigs;

// enumerador que auxilia no retorno do endereço de uma porta
// como PxIN, PxOUT...
typedef enum { in=0x00, out=0x02, dir=0x04, ren=0x06, ds=0x08, sel= 0x0A,
                ies=0x18, ie=0x1A, ifg=0x1C} pinTypes;

// enumerador para escolher a transição de interrupção (IES)
// low -> high
// high -> low
typedef enum {low_high=0, high_low=1} edgeSelect;

// Struct do pino define um pino P(port).(bit)
typedef struct {
    uint8_t port;
    uint8_t bit;
} Pin;

/*----------- Funções de configuração do pino ------------*/
/*                                            *
* Funções configuram os pinos de 2 formas:    *
*   1. Pela struct Pin                        *
*   2. Pela formacão do pino como P(PORT).bit *
*                                             */
// Configura o modo de um pino como input, output, inPullUP ou inPullDown
// Obs: O SEL do pino é automaticamente zerado
// pinConfig(1, 0, outPut) irá configurar P1.0 como saída
void pinConfig(uint8_t port, uint8_t bit, pinConfigs mode);
// Dual de pinConfig utilizando a struct Pin {port , bit} : P(port).(bit)
// pinConfigPin(Pin{1,0}, output) configura P1.0 como saída
void pinConfigPin(Pin * pin, pinConfigs mode);

// Configura as portas SEL, DIR, REN e OUT de um Pin
void pinStdConfig(Pin * pin, uint8_t v_sel, uint8_t v_dir, uint8_t v_ren, uint8_t v_out);

// Configura as portas SEL, DIR, REN, OUT, IE, IES de um Pin
void pinFullConfig(Pin * pin, uint8_t v_sel, uint8_t v_dir, uint8_t v_ren, uint8_t v_out, uint8_t v_ie, uint8_t v_ies);
/**********************************************/

/*-------------- Funções de leitura do pino --------------*/
// Retorna o valor de um pino específico
// digitalRead(1, 0) : valor em P1.0
uint8_t digitalRead(uint8_t port, uint8_t bit);
// Leitura do pino especificado pela struct Pin
// digitalRead(Pin{1,0}) : valor em P1.0
uint8_t digitalReadPin(Pin * pin);

// Escrita de um valor num pino específico
// Utilizada para output
// digitalWrite(1,0, 1) :: Seta P1.0
void digitalWrite(uint8_t port, uint8_t bit, uint8_t value);
// Escrita de um valor num pino específico
// digitalWritePin(Pin{1,0}, 1) :: Seta P1.0
void digitalWritePin(Pin * pin, uint8_t value);
/**********************************************/

/*--------------- Funções de Interrupções ----------------*/
// Configura o pino para receber interrupções
// configInterruptibleAddr(1,0,high_low) :: habilita interrupções 
// em P1.0 na transição 1->0
void configInterruptibleAddr(uint8_t port, uint8_t bit, edgeSelect edge);

// Configura interrupções no pino definido pela sruct Pin
// configInterruptiblePin(Pin{1,0},high_low) :: habilita interrupções 
// em P1.0 na transição 1->0
void configInterruptiblePin(Pin * pin, edgeSelect edge);
/****************************************************/

/*------------ Função de retorno do endereço -------------*/
// Retorna o endereço da porta de um pino de tipo específico
// getPin(1, in) : P1IN
uint8_t* getPin(uint8_t port, pinTypes type);
/**********************************************/

/*----------------- Funções Secundárias ------------------*/
/*                                                  *
* Set/Clear de um único bit do pino                 *
* O bit a ser setado/zerado é definido pela máscara *
*                                                   */
// Seta um único bit de determinado registrador do pino
// setSinglePinReg(tSEL, 1<<bit) :: seta tSEl.BIT
void setSinglePinReg(uint8_t * pinAddr, uint8_t mask);

// Zera um único bit de determinado registrador do pino
// clearSinglePinReg(tSEL, 1<<bit) :: zera tSEl.BIT
void clearSinglePinReg(uint8_t * pinAddr, uint8_t mask);
/****************************************************/

/*                                               *
* Configura o bit do pino com um valor escolhido *
*                                                */
// Configura o endereço de um pino (pinXAddr.bit) para o valor 
// especificado em val
void pinConfigAddr(uint8_t * pinAddr, uint8_t bit, uint8_t val);

// Configura o endereço de um pino pinXAddr para o valor 
// especificado pela máscara
void pinMaskConfigAddr(uint8_t * pinAddr, uint8_t mask);
/*************************************************/

/*------------------ Funções Auxiliares ------------------*/
void setAsInput(uint8_t mask, uint8_t * pinDir, uint8_t * pinRen, uint8_t * pinOut);
void setAsOutput(uint8_t mask, uint8_t * pinDir, uint8_t * pinOut);
void setAsInPUP(uint8_t mask, uint8_t * pinDir, uint8_t * pinRen, uint8_t * pinOut);
void setAsInPDwn(uint8_t mask, uint8_t * pinDir, uint8_t * pinRen, uint8_t * pinOut);
void setAsIE(uint8_t mask, uint8_t * pinIE);
void setAsIES(uint8_t mask, uint8_t * pinIES, uint8_t edge);

#endif /* LIBS_GPIO_H_ */
