/*
 *  Tarefa 04: Uso do WDT
 *
 *  Created on: Jul 10, 2021
 *      Author: Nycolas Coelho de abreu
 *      Instituto Federal de Santa Catarina
 *
 *      Utilização do WDT para uso como delay, utilizando interrupções e
 *      clock do sistema.
 *
 *      A função delay_ms tem 1ms por tick e no máximo 32767ms(32,767s) de delay
 *      devido a sua resolução de 16 bits.
 *
 *      Utilizando a função delay_ms(), foi utilizado 56,2mJ durante 10s
 *      Utilizando a função __delay_cycles(), foi utilizado 63,3mJ durante 10s
 *
 *             MSP430FR2355 - LaunchPad
 *               -----------------
 *           /|\|              XIN|-
 *            | |                 |
 *            --|RST          XOUT|-
 *              |                 |
 *      LED <-- | P1.0            |
 *
 */

#ifndef __MSP430FR2355__
    #error "Clock system not supported for this device"
#endif

#include <msp430.h>
#include <stdint.h>
#include "WDT_as_timer.h"

#define LED BIT0                // Define o led da placa de desenvolvimento que é P1.0

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // Desliga watchdog

#if defined (__MSP430FR2355__)
    PM5CTL0 &= ~LOCKLPM5;       // Disable the GPIO power-on default high-impedance mode
#endif

    P1DIR = LED;                // LED como saída

    while(1){
        P1OUT |= LED;           // Liga LED 1
        delay_ms(100);         // Delay em ms
        P1OUT&= ~LED;           // Desliga LED 1
        delay_ms(100);
    }
}
