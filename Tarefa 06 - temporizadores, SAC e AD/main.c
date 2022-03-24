/*
 *  Tarefa 06: Temporizadores, SAC e AD
 *
 *  Created on: Jul 29, 2021
 *      Author: Nycolas Coelho de abreu
 *      Instituto Federal de Santa Catarina
 *
 *             MSP430FR2355 - LaunchPad
 *               -----------------
 *           /|\|              XIN|-
 *            | |                 |
 *            --|RST          XOUT|-
 *              |                 |
 *          <-- | P1.0       P2.0 | -->
 *          <-- | P6.6            |
 *          <-- | P2.3            |
 *          <-- | P4.1            |
 *
 */

#include <msp430.h>
#include "SAC.h"
#include "ADC.h"
#include "Timer.h"
#include "CLK.h"

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	PM5CTL0 &= ~LOCKLPM5;       // Disable the GPIO power-on default high-impedance mode
	
	P1DIR |= BIT0;                  // LED DEBUG
	P6DIR |= BIT6;                  // LED Sensor de luz

	init_CLK();   //testar com 1M primeiro
	init_SAC();
	init_ADC();
	init_TIMERB1();

	while (1)
    {
        /* Desliga CPU até ADC terminar */
        __bis_SR_register(CPUOFF + GIE);
    }
}
