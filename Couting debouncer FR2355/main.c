/*
 * 05_main_couting_deboucer.c
 *
 *  Created on: Mar 27, 2020
 *      Author: Renan Augusto Starke
 *      Instituto Federal de Santa Catarina
 *
 *      Exemplo de debouce de botão por pooling.
 *      Faz a verificação de botão periodicamente utilizando
 *      o comparador 0 do Timer B0
 *      Período do pooling deve ser maior que o tempo de
 *      instabilidade da chave.
 *
 *       .
 *      /|\                  +
 *       |                 +
 *       |               +
 *       |             +  <-- Comparador 0 (TBCCR0  -> TIMER0_B0_VECTOR) - Debouncer do botão
 *       |           +
 *       |         +
 *       |       +
 *       |     +
 *       |   +
 *       | +
 *       +----------------------------------------->
 *
 *       - Comparadores podem ser configurados para qualquer valor
 *       entre 0 e 65535. IRQs devem ser habilitadas individuais
 *       nos respectivos registradores.
 *
 */

/* System includes */
#include <msp430.h>
#include <stdint.h>

/* Project includes */
#include "gpio.h"
#include "bits.h"

#define BUTTON_PORT P2
#define BUTTON BIT3

#define LED1 BIT0
#define LED2 BIT6

#define BUTTON_SAMPLES (5)  // Para clock default de 1MHz fica melhor fazer menos samples

void init_clock_system(){

    // Configure two FRAM wait state as required by the device data sheet for MCLK
    // operation at 24MHz(beyond 8MHz) _before_ configuring the clock system.
    FRCTL0 = FRCTLPW | NWAITS_2 ;

    P2SEL1 |= BIT6 | BIT7;                       // P2.6~P2.7: crystal pins
    do
    {
        CSCTL7 &= ~(XT1OFFG | DCOFFG);           // Clear XT1 and DCO fault flag
        SFRIFG1 &= ~OFIFG;
    } while (SFRIFG1 & OFIFG);                   // Test oscillator fault flag

    __bis_SR_register(SCG0);                     // disable FLL
    CSCTL3 |= SELREF__XT1CLK;                    // Set XT1 as FLL reference source
    CSCTL0 = 0;                                  // clear DCO and MOD registers
    CSCTL1 = DCORSEL_7;                          // Set DCO = 24MHz
    CSCTL2 = FLLD_0 + 731;                       // DCOCLKDIV = 327358*731 / 1
    __delay_cycles(3);
    __bic_SR_register(SCG0);                     // enable FLL
    while(CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1));   // FLL locked

    /* CSCTL4 = SELMS__DCOCLKDIV | SELA__REFOCLK;
     * set XT1 (~32768Hz) as ACLK source, ACLK = 32768Hz
     * default DCOCLKDIV as MCLK and SMCLK source
     - Selects the ACLK source.
     * 00b = XT1CLK with divider (must be no more than 40 kHz)
     * 01b = REFO (internal 32-kHz clock source)
     * 10b = VLO (internal 10-kHz clock source) (1) <<< USANDO ESSE   */
    CSCTL4 = SELMS__DCOCLKDIV | SELA__REFOCLK;
}

/**
  * @brief  Configura temporizador A.
  *
  * @param  none
  *
  * @retval none
  */
void config_timerB_0(){
    /* Timer B0:
     *
     * TASSEL_2 -> Clock de SMCLK.
     * MC_2 -> Contagem crescente.
     * ID_3 -> Prescaler = /8
     */

    TB0CTL = TBSSEL_2 | MC_2 | ID_2;

    /* IRQ por comparação entre contagem e comparador 0 */
    TB0CCTL0 = CCIE;
    /* Valor de comparação é 50000 */
    TB0CCR0 = 20000;
}

int main(void)
{
    volatile uint16_t my_data = 0;

    /* Desliga watchdog imediatamente */
    WDTCTL = WDTPW | WDTHOLD;
    PM5CTL0 &= ~LOCKLPM5;

    /* Configura botões */
    /* BUTTON_PORT totalmente como entrada */
    P1DIR |= LED1;      // LED1 como saida
    P6DIR |= LED2;      // LED2 como saida

    /* Resistores de pull up */
    PORT_REN(BUTTON_PORT) = BUTTON;
    PORT_OUT(BUTTON_PORT) = BUTTON;

    /* Configurações de hardware */
    init_clock_system();
    config_timerB_0();

    /* Entra em modo de economia de energia com IRQs habilitadas */
    __bis_SR_register(LPM0_bits + GIE);

    while (1){

        /* Desligar CPU novamente */
       __bis_SR_register(LPM0_bits);
    }
}

/* ISR0 do Timer B: executado no evento de comparação  comparador 0 (TBCCR0)
 *
 * Utilizado para o debouncer por amostragem: faz a verificação de botão
 * periodicamente.
 *
 * */
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_B0_VECTOR
__interrupt void TIMER0_B0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_B0_VECTOR))) Timer_B (void)
#else
#error Compiler not supported!
#endif
{
    static uint8_t counter = BUTTON_SAMPLES;

    /* Debug: Piscar quando executar a ISR */
    CPL_BIT(P6OUT, BIT6);

    /* Se botão apertado: borda de descida */
    if (!TST_BIT(PORT_IN(BUTTON_PORT), BUTTON))  {
        /* Se contagem = 0, debounce terminado */
        if (!(--counter)) {

            /* Colocar aqui código da aplicação referente ao botão */

            /* Acorda função main
            __bic_SR_register_on_exit(LPM0_bits); */

            CPL_BIT(PORT_OUT(P1), LED1);
        }
    }
    else
        counter = BUTTON_SAMPLES;
}
