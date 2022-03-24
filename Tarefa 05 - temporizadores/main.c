/*
 *  Tarefa 05: Temporizadores
 *
 *  Created on: Jul 21, 2021
 *      Author: Nycolas Coelho de abreu
 *      Instituto Federal de Santa Catarina
 *
 *      Utiliza��o do Timer para uso como PWM, utilizando bot�es,
 *      interrup��es e counting debouncer para incremento
 *      e decremento do PWM.
 *
 *             MSP430FR2355 - LaunchPad
 *               -----------------
 *           /|\|              XIN|-
 *            | |                 |
 *            --|RST          XOUT|-
 *              |                 |
 *     LED1 <-- | P1.0       P2.0 | --> PWM
 *     LED2 <-- | P6.6            |
 *  BUTTON1+<-- | P2.3            |
 *  BUTTON2-<-- | P4.1            |
 *
 */

#include <msp430.h>
#include "clk.h"
#include "timer.h"

#define BUTTON1 BIT3
#define BUTTON2 BIT1

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;       // stop watchdog timer

#if defined(__MSP430FR2355__)
    PM5CTL0 &= ~LOCKLPM5;           // Disable the GPIO power-on default high-impedance mode
#endif

    init_clock_system();            // Inicia SMCLK em 24MHz
    config_timerB_1_as_pwm();       // Liga��o f�sica do timer pwm (TB1.1 � o P2.0)
    config_timerB_0_debouncer();    // Inicia o timer TB0 como debouncer

    P2DIR = BIT0;                   // Liga��o P2.0 (TIMER TB1.1) como sa�da
    P2SEL0 = BIT0;                  // Liga fun��o alternativa do pino de sa�da para timer ativando TB1.1

    P1DIR |= BIT0;                  // LED DEBUG
    P6DIR |= BIT6;                  // LED DEBUG

    P2REN |= BUTTON1;                // Resistores de pull up para bot�es
    P4REN |= BUTTON2;
    P2OUT |= BUTTON1;
    P4OUT |= BUTTON2;

    __bis_SR_register(LPM0_bits + GIE);

    while (1)
    {
        __bis_SR_register(LPM0_bits + GIE);
        /* C�digo de baixa prioridade da aplica��o */
    }
}
