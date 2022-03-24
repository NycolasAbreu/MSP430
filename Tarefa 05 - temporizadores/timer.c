/*
 * timer.c
 *
 *  Created on: 21 de jul de 2021
 *      Author: Nycolas
 */

#include "timer.h"

#define BUTTON_SAMPLES (12) // Define a quantidade de amostras do debouncer

/**
 * @brief  Configura temporizador B1 com contagem up e down.
 *
 * @param  none
 *
 * @retval none
 */
void config_timerB_1_as_pwm(void)
{
    /* Estamos usando TB1CCR0 para contagem m�xima
     * que permite controle preciso sobre o per�odo
     * � poss�vel usar o overflow */

    /* Configura��o dos comparadores como PWM:
     *
     * TB1CCR0: Timer1_B Capture/Compare 0: per�odo do PWM
     *
     * OUTMOD_2: PWM output mode: 2 - PWM toggle/reset
     *
     * TB1CCR1 PWM duty cycle: TB1CCR1 / TB1CCR0
     *
     * Calculo do overflow comparador 0
     *
     *  - Periodo da frequencia escolhida
     *  T = 1 / 10kHz = 0.0001
     *  - Multiplique pela frequencia do timer
     *  Resolu��omax = T * 24MHz = 2400
     *  - Como � Up/Down devemos dividir por 2
     *  Resolu��oUpDown = 2400 / 2 = 1200
    */

    TB1CCR0 = 1200 - 1;

    TB1CCTL1 = OUTMOD_2;

    TB1CCR1 = 600; // Come�a em 50%

    /*      .
    *      /|\                  +                < -Comparador 0: (m�ximo da contagem) -> per�odo do PWM
    *       |                 +   +
    *       |               +       +
    *       |-------------+---------- +          <--  Comparador 1: raz�o c�clica
    *       |           +  |         | +
    *       |         +    |         |   +
    *       |       +      |         |     +
    *       |     +        |         |       +
    *       |   +          |         |         +
    *       | +            |         |           +
    * Timer +--------------|---- ----|-------------->
    *       |              |
    *       |
    *
    *       |--------------+         |--------------
    * Sa�da |              |         |
    *       +---------------++++++++++------------->
    */

    /* TBSSEL_2 -> Timer B clock source select: SMCLK
     * MC_3     -> Up/down mode: Timer counts up to TBxCL0 and down to 0000h
     * ID_0     -> Timer A input divider: divide by 1
     *
     * Configura��o da fonte do clock do timer 1 */
    TB1CTL = TBSSEL_2 | MC_3 | ID_0;
}

/**
 * @brief  Configura temporizador B0 para uso do debouncer.
 *
 * @param  none
 *
 * @retval none
 */
void config_timerB_0_debouncer(void)
{
    /* Timer B0:
     *
     * TASSEL_2 -> Clock de SMCLK.
     * MC_2 -> Contagem crescente.
     * ID_3 -> Prescaler = /8
     */
    TB0CTL = TBSSEL_2 | MC_2 | ID_2;

    /* IRQ por compara��o entre contagem e comparador 0 */
    TB0CCTL0 = CCIE;
    /* Valor de compara��o at� 20000 */
    TB0CCR0 = 20000;
}

static inline void inc_pwm(){
    if (TB1CCR1 <= 1199)
    {
        TB1CCR1 = TB1CCR1 + 59; // Incrementos de 5%
        CPL_BIT(P1OUT, BIT0);   // Debug: Piscar quando executar a ISR
        //__bic_SR_register_on_exit(LPM0_bits); // Acorda fun��o main
    }
    else
    {
        TB1CCR1 = 0;            // Reseta a contagem para 0
        CPL_BIT(P1OUT, BIT0);   // Debug: Piscar quando executar a ISR
        //__bic_SR_register_on_exit(LPM0_bits); // Acorda fun��o main
    }
}

static inline void dec_pwm(){
    if (TB1CCR1 > 1)
    {
        TB1CCR1 = TB1CCR1 - 59; // Decrementos de 5%
        CPL_BIT(P1OUT, BIT0);   // Debug: Piscar quando executar a ISR
    }
    else
    {
        TB1CCR1 = 1198;         // Reseta a contagem para 0
        CPL_BIT(P1OUT, BIT0);   // Debug: Piscar quando executar a ISR

    }
}

/* ISR0 do Timer B0: executado no evento de compara��o comparador 0 (TB0CCR0)
 *
 * Utilizado para o debouncer por amostragem: faz a verifica��o de bot�o
 * periodicamente.
 *
 * */
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER0_B0_VECTOR
__interrupt void TIMER0_B0_ISR(void)
#elif defined(__GNUC__)
void __attribute__((interrupt(TIMER0_B0_VECTOR))) Timer_B(void)
#else
#error Compiler not supported!
#endif
{
    static uint8_t counter = BUTTON_SAMPLES;

    /* Debug: Piscar quando executar a ISR */
    CPL_BIT(P6OUT, BIT6);

    /* Se bot�o 1 apertado: borda de descida */
    if (!TST_BIT(P2IN, BIT3))
    {
        // Se contagem = 0, debounce terminado
        if (!(--counter))
        {
            inc_pwm();
            //__bic_SR_register_on_exit(LPM0_bits); // Acorda fun��o main
        }
    }
    else if (!TST_BIT(P4IN, BIT1))
    {
        /* Se contagem = 0, debounce terminado */
        if (!(--counter))
        {
            dec_pwm();
            //__bic_SR_register_on_exit(LPM0_bits); // Acorda fun��o main
        }
    }
    else
        counter = BUTTON_SAMPLES;
}
