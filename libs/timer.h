/*
 * timer.h
 *
 *  Created on: Oct 2, 2020
 *      Author: ricardosantos
 */

#ifndef LIBS_TIMER_H_
#define LIBS_TIMER_H_

#include <stdint.h>

/* Deinição de Tipos */
// enumerador que define os tipos possíveis de tempo para a função wait()
typedef enum {us, ms, sec} timeUnit_t;

// enumerador que define a posição base na memória de cada timer A e B
typedef enum {TA_0=0x340, TA_1=0x380, TA_2=0x400, TB_0=0x3C0} timerPin;

// enumerador que define o offset para cada registrador do timer
typedef enum {CTL=0x00, CCTL=0x02, R=0x10, CCR=0x12, IV=0x2E, 
                EX0=0x20} timerRegister;

// enumerador que define os ports do registrador CTL do timer, IFG, IE...
typedef enum {ctl_IFG=0, ctl_IE=1, ctl_CLR=2, ctl_MC=4, ctl_ID=6,
                ctl_SEL=8} timerCTLPorts;

// enumerador que define os ports do registrador CCTL do timer, ccifg, cap...
typedef enum {cctl_ccifg=0, cctl_cov=1, cctl_out=2, cctl_cci=3, cctl_ccie=4,
            cctl_outmod=5, cctl_cap=8, cctl_scci=10, cctl_scs=11, cctl_ccis=12,
            cctl_cm=14} timerCCTLPorts;

// enumerador que define os tipos de clocks do microcontrolador
// ACLK = 32768Hz
// SMCLK = 1_048_576Hz
typedef enum {ACLK=1, SMCLK=2} timerClock;

// enumerador que define os modos de contagem do clock
typedef enum {Stop, UP, Continuous, UP_Down} timerMode;

// enumerador que define as divisões da frequência de clock da porta IE do CTL
typedef enum {one=0, two=1, four=2, eight=3} inputDivider;

// enumerador que define onde será feita a captura: bit CM de CCTLx
typedef enum {noCapture=0, risingEdge=1, fallingEdge=2, bothEdges=3} edgeMode;

// enumerador que define a captura: bit CCIS e CCTLx
typedef enum {cciA=0, cciB=1, gnd=2, vcc=3} inSlct;

// enumerador que decide se será feita a comparação ou captura
typedef enum {compare=0, capture=1} capMode;

// enumerador que define o modo de saída: bit OUTMOD de CCTLx
typedef enum {set=1, toggle_reset=2, set_reset=3, toggle=4, reset=5, toggle_set=6,
                reset_set=7 } outMod;

// struct que define a base, eg: TA_0... do pino e o registrador: CTL, CCR...
typedef struct {
    timerPin base;
    timerRegister reg;
} Timer;

/*---------------- Funções Principais -----------------*/

// Aguarda um determinado tempo em determinado timer
// wait(5, sec, TA_0) :: Aguarda 5 segundos no timer A com CCTL0
void wait(uint16_t time, timeUnit_t unit, timerPin timer);

// Retona o registrador especificado do timer escolhido
// O timer é criado pela struct Timer com os campos base, eg: TA_0  e 
// registrador, eg: CTL
// A função com offset deve ser usada no caso de multiplos registradores, eg: CCR2
uint16_t * getTimerRegstr(Timer timer);
// Dual da função sem Off
// Essa função deve ser utilizada com registradores CCR e CCTL
// O timer é criado pela struct Timer com os campos base, eg: TA_0  e 
// registrador, eg: CTL
uint16_t * getTimerRegstrOff(Timer timer, uint8_t offset);

// Configura uma interrupção na porta CCR específicada
// O timer na função dual deve ser criado pela struct Timer com TA_X e CCR
void setInterruptionAt(Timer timer, uint8_t off, uint16_t time);
// Configura uma interrupção na porta CCR específicada
// O pino CCR deve ser obtida via a função getTimerRegstOff();
void setInterruptionAtAddr(uint16_t * timerCCR, uint16_t time);

/*-----------Configura a porta CTL-----------*/
// ctl FullConfig configura: SEL, ID, MC e IE
void ctlFullConfigAddr(uint16_t * timer, timerClock sel, inputDivider id, timerMode mc, uint8_t ie);
// ctl QuickConfig configura: MC e SEL
void ctlQuickConfigAddr(uint16_t * timer, timerMode mc, timerClock sel);
// ctl StdConfig configura: MC, ID e IE
void ctlStdConfigAddr(uint16_t * timer, timerMode mc, inputDivider id, timerClock sel);

// ctl FullConfig configura: IE, MC, ID e SEL
void ctlFullConfig(Timer timerS, uint8_t ie, timerMode mc, inputDivider id, timerClock sel);
// ctl FullConfig configura: MC e SEL
void ctlQuickConfig(Timer timerS, timerMode mc, timerClock sel);
// ctl FullConfig configura: MC, ID e SEL
void ctlStdConfig(Timer timerS, timerMode mc, inputDivider id, timerClock sel);

/*-----------Configura a porta CCTL-----------*/
// A configuração desse registrador pode ser feito de duas formas:
//  Pelo Endereço, obtido pela função getTimerRegstr ou
// Pela struct Pin{base, CTL}

// cctl FullConfig configura: CM, CCIS, SCS, CAP, OUTMOD, CCIE, OUT
void cctlFullConfigAddr(uint16_t * timer, edgeMode cm, inSlct ccis, uint8_t scs, capMode cap, outMod om, uint8_t ccie, uint8_t out);
// cctl CompInterrupt configura: CCIE
void cctlCompInteruptConfigAddr(uint16_t * timer, uint8_t ccie);
// cctl CapQuick configura: CM e CAP
void cctlCapQuickConfigAddr(uint16_t * timer, edgeMode cm, capMode cap);
// cctl CapStd configura: CM, CCIS, SCS, CAP, CCIE
void cctlCapStdConfigAddr(uint16_t * timer, edgeMode cm, inSlct ccis, uint8_t scs, capMode cap, uint8_t ccie);

// cctl FullConfig configura: CM, CCIS, SCS, CAP, OUTMOD, CCIE, OUT
void cctlFullConfig(Timer timerS, edgeMode cm, inSlct ccis, uint8_t scs, capMode cap, outMod om, uint8_t ccie, uint8_t out);
// cctl CompInterrupt configura: CCIE
void cctlCompInteruptConfig(Timer timerS, uint8_t offset, uint8_t ccie);
// cctl CapQuick configura: CM e CAP
void cctlCapQuickConfig(Timer timerS, edgeMode cm, capMode cap);
// cctl CapStd configura: CM, CCIS, SCS, CAP, CCIE
void cctlCapStdConfig(Timer timerS, edgeMode cm, inSlct ccis, uint8_t scs, capMode cap, uint8_t ie);

/*-----------Configura a porta EX0-----------*/
void ex0ConfigAddr(uint16_t * timerEX0, uint8_t divideBy);
void ex0Config(Timer timer, uint8_t divideBy);

/*-----------Funções Auxiliares-----------*/
// Trava operações no timer e cctl_N após configurado ccr e ctl
// ccr e ctl podem ser configurados por configTimer() e setInterruption
// ou manualmente
// await(TA_1, 2) :: Aguarda o tempo especificado em ta1ccr2
void await(timerPin timer, uint8_t cctl_N);

// Reinicia e para o timer
// stopTimer(timerCTL) :: zera e para o timer especificado
// timerCTL deve ser obtido por getTimerRegstr()
void stopTimer(uint16_t * timerCTL);


#endif /* LIBS_TIMER_H_ */







