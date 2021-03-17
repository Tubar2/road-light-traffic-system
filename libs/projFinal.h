/*
 * projFinal.h
 */

#include <stdint.h>
#include "libs/gpio.h"
#include "libs/timer.h"

#ifndef LIBS_PROJFINAL_H_
#define LIBS_PROJFINAL_H_

#define LIGHT_TIME 3, sec

typedef struct {
    Pin r;
    Pin g;
    Pin y;
} Semaforo;

typedef enum {state1, state2} semState;


// Faz o processo da troca de estados onde:
// Semaforo Atual fecha (verde -> amarelo -> vermelho)
// e proximo Semaforo abre (vermelho -> verde)
void switchSemState(Semaforo * cur, Semaforo * prox);

// Configura os pinos do semaforo (Comeca com todos desligados)
void configSem(Semaforo * sem);

// Configura o estado inicial dos semaforos na simulation onde:
// s1 comeca aberto (verde ligado)
// s2 comeca fechado (vermelho ligado)
void initSemState(Semaforo * s1, Semaforo * s2);

void printUARTnum(uint8_t num);

#endif /* LIBS_PROJFINAL_H_ */
