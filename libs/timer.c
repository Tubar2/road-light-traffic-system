/*
 * timer.c
 *
 *  Created on: Oct 2, 2020
 *      Author: ricardosantos
 */

#include <msp430.h>
#include "timer.h"


void wait(uint16_t time, timeUnit_t unit, timerPin timer)
{
    const uint8_t regN = 0;     // Define o CCR_n e o CCTL_n

    // Endereço do registrador CTL do timer escolhido (TxCTL)
    Timer t = {timer, CTL};
    uint16_t * timerCTL = getTimerRegstr(t);

    // Endereço do registrador CCR do timer e n escolhidos (TxCCRn)
    t.reg = CCR;
    uint16_t * timerCCR = getTimerRegstrOff(t, regN);

    switch (unit){

    case us:
        // Configura o timer escolhido (TA0, TA1 ou ...) com SMCLK, MC_UP e InputDiv = 0
        ctlQuickConfigAddr(timerCTL, UP, SMCLK);
        // configTimer(timerCTL, SMCLK, UP, 0);

        // Adiciona uma interrução no TxCCRn escolhido no tempo determinado
        setInterruptionAtAddr(timerCCR, time);

        // Aguarda a interrupção acontecer
        await(timer, regN);

        // Para e reinicia o timer
        stopTimer(timerCTL);
        break;

    case ms:
    {
        // Divide o clock de (32MHz por 8) =~ 4 ms
        Timer t = {timer, EX0};
        uint16_t * timerEX0 = getTimerRegstr(t);
        ex0ConfigAddr(timerEX0, 7);
        // setupTimerRegister(timerEX0, 7, none);

        // Configura o timer com ACLK, MC_UP e InputDivider = 4
        ctlStdConfigAddr(timerCTL, UP, four, ACLK);
        // configTimer(timerCTL, ACLK, UP, 3);

        // Adiciona uma interrução no TxCCRn escolhido no tempo determinado
        setInterruptionAtAddr(timerCCR, time);

        // Aguarda a interrupção acontecer
        await(timer, regN);

        // Para e reinicia o timer
        stopTimer(timerCTL);
        ex0ConfigAddr(timerEX0, 0);
    }
        break;

    case sec:
    {
        // Configura o timer escolhido com ACLK e MC_UP
        ctlQuickConfigAddr(timerCTL, UP, ACLK);
        // configTimer(timerCTL, ACLK, UP, 0);

        // Define variáveis auxiliares
        uint16_t timer_sec = 0;             // Batidas de 1 segundo
        const uint16_t oneSec = 0x8000 - 1; // Um segundo com o clock ACLK

        // Adiciona uma interrupção em 1 segundo
        setInterruptionAtAddr(timerCCR, oneSec);

        // Loop que roda time segundos
        while(timer_sec != time) {

            // Aguarda um segundo
            await(timer, regN);

            // Toda vez que passa um segundo, incrementa o timer de segundos
            timer_sec++;
        }

        // Para e reinicia o timer
        stopTimer(timerCTL);
    }
        break;
    }

}

uint16_t * getTimerRegstr(Timer timer){
    // Forma o endereço do registrador de 16 bits escolhido a partir da
    uint16_t * regstr =
            (uint16_t *) (
                    timer.base    +       // base do timer    eg. TA0 +
                    timer.reg             // registrador      eg. SEL +
            );

    return regstr;
}

uint16_t * getTimerRegstrOff(Timer timer, uint8_t offset){

    // Forma o endereço do registrador de 16 bits escolhido a partir da
    uint16_t* regstr =
            (uint16_t *) (
                    timer.base    +       // base do timer    eg. TA0 +
                    timer.reg     +       // registrador      eg. SEL +
                    (2*offset)      // offset para os registrador CCTLn/CCRn
            );

    return regstr;
}

void setInterruptionAt(Timer timer, uint8_t off, uint16_t time){
    uint16_t * timerCCR = getTimerRegstrOff(timer, off);
    *timerCCR = time - 1;
}

void setInterruptionAtAddr(uint16_t * timerCCR, uint16_t time){
    // Configura CCRn com o tempo especificado, demarcando uma interrupção
    *timerCCR = time - 1;
}

void await(timerPin timer, uint8_t cctl_N) {
    // Pega o endereço da porta CCTL do timer
    Timer t = {timer, CCTL};
    uint16_t * timerCCTL = getTimerRegstrOff(t, cctl_N);

    // Enquanto a flag de interrupção não for setada, trava o programa
    while(!(*timerCCTL & CCIFG));

    // Limpa a flag para que novas interrupções possam acontecer
    *timerCCTL &= ~CCIFG;

}

void stopTimer(uint16_t * timerCTL){
    *timerCTL = MC__STOP | TACLR;
}
/*----------Funções para configurar o registrador CTL de um timer------------*/
// ctlFullConfigAddr configura todas as portas do registrador CTL do timer
void ctlFullConfigAddr(uint16_t * timer, timerClock sel, inputDivider id, timerMode mc, uint8_t ie){
    uint16_t maskSEL = sel << ctl_SEL;
    uint16_t maskID = id << ctl_ID;
    uint16_t maskMC = mc << ctl_MC;
    uint16_t maskIE = ie << ctl_IE;

    *timer |= maskIE | maskMC | maskID | maskSEL;
}

// ctlQuickConfigAddr configura as portas MC e SEL do registrador CTL do Timer
void ctlQuickConfigAddr(uint16_t * timer, timerMode mc, timerClock sel) {
    uint16_t maskSEL = sel << ctl_SEL;
    uint16_t maskMC = mc << ctl_MC;

    *timer |= maskMC | maskSEL;
}

// ctlQuickConfigAddr configura as portas MC, ID e SEL do registrador CTL do timer
void ctlStdConfigAddr(uint16_t * timer, timerMode mc, inputDivider id, timerClock sel){
    uint16_t maskSEL = sel << ctl_SEL;
    uint16_t maskID = id << ctl_ID;
    uint16_t maskMC = mc << ctl_MC;

    *timer |= maskMC | maskID | maskSEL;
}

// ctlFullConfigAddr configura todas as portas do registrador CTL do timer
void ctlFullConfig(Timer timerS, uint8_t ie, timerMode mc, inputDivider id, timerClock sel){
    uint16_t * timer = getTimerRegstr(timerS);

    uint16_t maskSEL = sel << ctl_SEL;
    uint16_t maskID = id << ctl_ID;
    uint16_t maskMC = mc << ctl_MC;
    uint16_t maskIE = ie << ctl_IE;

    *timer |= maskIE | maskMC | maskID | maskSEL;
}

// ctlQuickConfigAddr configura as portas MC e SEL do registrador CTL do Timer
void ctlQuickConfig(Timer timerS, timerMode mc, timerClock sel) {
    uint16_t * timer = getTimerRegstr(timerS);

    uint16_t maskSEL = sel << ctl_SEL;
    uint16_t maskMC = mc << ctl_MC;

    *timer |= maskMC | maskSEL;
}

// ctlQuickConfigAddr configura as portas MC, ID e SEL do registrador CTL do timer
void ctlStdConfig(Timer timerS, timerMode mc, inputDivider id, timerClock sel){
    uint16_t * timer = getTimerRegstr(timerS);

    uint16_t maskSEL = sel << ctl_SEL;
    uint16_t maskID = id << ctl_ID;
    uint16_t maskMC = mc << ctl_MC;

    *timer |= maskMC | maskID | maskSEL;
}

/*----------Funções para configurar o registrador CCTL de um timer-----------*/
// cctlFullConfigAddr configuraça todas as portas do registrador CCTL do timer
void cctlFullConfigAddr(uint16_t * timer, edgeMode cm, inSlct ccis, uint8_t scs, capMode cap, outMod om, uint8_t ccie, uint8_t out){
    uint16_t maskCM = cm << cctl_cm;
    uint16_t maskCCIS = ccis << cctl_ccis;
    uint16_t maskSCS = scs << cctl_scs;
    uint16_t maskCAP = cap << cctl_cap;
    uint16_t maskOM = om << cctl_outmod;
    uint16_t maskCCIE = ccie << cctl_ccie;
    uint16_t maskOUT = out << cctl_out;

    *timer |= maskOUT | maskCCIE | maskOM | maskCAP | maskSCS | maskCCIS | maskCM;
}

// cctlCompInterruptConfigAddr configuraça a porta ccie do registrador CCTL do timer
void cctlCompInteruptConfigAddr(uint16_t * timer, uint8_t ccie) {
    uint16_t maskCCIE = ccie << cctl_ccie;

    *timer |= maskCCIE;
}

//
void cctlCapQuickConfigAddr(uint16_t * timer, edgeMode cm, capMode cap){
    uint16_t maskCM = cm << cctl_cm;
    uint16_t maskCAP = cap << cctl_cap;

    *timer |= maskCM | maskCAP;
}

void cctlCapStdConfigAddr(uint16_t * timer, edgeMode cm, inSlct ccis, uint8_t scs, capMode cap, uint8_t ccie){
    uint16_t maskCM = cm << cctl_cm;
    uint16_t maskCCIS = ccis << cctl_ccis;
    uint16_t maskSCS = scs << cctl_scs;
    uint16_t maskCAP = cap << cctl_cap;
    uint16_t maskCCIE = ccie << cctl_ccie;

    *timer |= maskCCIE | maskCAP | maskSCS | maskCCIS | maskCM;
}

void cctlFullConfig(Timer timerS, edgeMode cm, inSlct ccis, uint8_t scs, capMode cap, outMod om, uint8_t ccie, uint8_t out){
    uint16_t * timer = getTimerRegstr(timerS);

    uint16_t maskCM = cm << cctl_cm;
    uint16_t maskCCIS = ccis << cctl_ccis;
    uint16_t maskSCS = scs << cctl_scs;
    uint16_t maskCAP = cap << cctl_cap;
    uint16_t maskOM = om << cctl_outmod;
    uint16_t maskCCIE = ccie << cctl_ccie;
    uint16_t maskOUT = out << cctl_out;

    *timer |= maskOUT | maskCCIE | maskOM | maskCAP | maskSCS | maskCCIS | maskCM;
}

void cctlCompInteruptConfig(Timer timerS, uint8_t offset, uint8_t ccie) {
    uint16_t * timer = getTimerRegstrOff(timerS, offset);

    uint16_t maskCCIE = ccie << cctl_ccie;

    *timer |= maskCCIE;
}

void cctlCapQuickConfig(Timer timerS, edgeMode cm, capMode cap){
    uint16_t * timer = getTimerRegstr(timerS);

    uint16_t maskCM = cm << cctl_cm;
    uint16_t maskCAP = cap << cctl_cap;

    *timer |= maskCM | maskCAP;
}

void cctlCapStdConfig(Timer timerS, edgeMode cm, inSlct ccis, uint8_t scs, capMode cap, uint8_t ccie){
    uint16_t * timer = getTimerRegstr(timerS);
    
    uint16_t maskCM = cm << cctl_cm;
    uint16_t maskCCIS = ccis << cctl_ccis;
    uint16_t maskSCS = scs << cctl_scs;
    uint16_t maskCAP = cap << cctl_cap;
    uint16_t maskCCIE = ccie << cctl_ccie;

    *timer |= maskCCIE | maskCAP | maskSCS | maskCCIS | maskCM;
}

/*----------Funções para configurar o registrador EX0 de um timer-----------*/
void ex0ConfigAddr(uint16_t * timerEX0, uint8_t divideBy){
    *timerEX0 = divideBy;
}

void ex0Config(Timer timer, uint8_t divideBy){
    uint16_t * timerEX0 = getTimerRegstr(timer);

    *timerEX0 = divideBy;
}





