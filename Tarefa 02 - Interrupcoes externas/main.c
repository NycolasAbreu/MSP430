/*  main.c
 *
 *      Created on: Jun 07, 2021
 *      Author: Nycolas Coelho de Abreu
 *      Instituto Federal de Santa Catarina
 *
 *      Contador hexadecimal com reset utilizando interrupcoes
 *      - Botão 1 para reset.
 *      - Botão 2 e 3 para alterar velocidade.
 *
 *                  MSP430G2553
 *               -----------------
 *           /|\|              XIN|-
 *            | |                 |
 *            --|RST          XOUT|-
 *              |                 |
 *  BUTTON1 <-- | P1.0            |
 *  BUTTON2 <-- | P1.1            |
 *  BUTTON3 <-- | P1.2            |
 *  DISPLAY <-- | P2.[0..7]       |
 */

#include <msp430.h>
#include <stdint.h>                             /* Tipos uint16_t, uint8_t, ... */
#include "led_display.h"
#include "bits.h"

#define BUTTON_1 BIT0
#define BUTTON_2 BIT1
#define BUTTON_3 BIT2
#define DELAY_FAST 10000
#define DELAY_SLOW 100000

volatile uint8_t display_number = 0;            /* Incremento do display */
volatile uint8_t boolean = 0;                   /* Boleano para delay */

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	                /* Para o watchdog timer */

	P1REN = BUTTON_1 | BUTTON_2 | BUTTON_3;     /* Configurando botoes em pull up */
	P1OUT = BUTTON_1 | BUTTON_2 | BUTTON_3;

	P1IES |= BUTTON_1 | BUTTON_2 | BUTTON_3;    /* Transição de nível alto para baixo */

	P1IE |= BUTTON_1 | BUTTON_2 | BUTTON_3;     /* Configurando interrupcao nos botoes */

	CLR_BIT(P1IFG,(BUTTON_1 | BUTTON_2 | BUTTON_3)); /* Limpa alguma IRQ pendente */

	display_init();                             /* Inicializa display modo anodo */

	//__bis_SR_register(GIE);                   /* Habilitar as interrupções, dois modos funcionam */
	__enable_interrupt();

	while(1)
	    {
	        display_write(display_number);

	        if (boolean)                        /* Se 1 então delay rapido Se 0 então delay devagar */
	            __delay_cycles(DELAY_FAST);
	        else
	            __delay_cycles(DELAY_SLOW);

	        display_number++;                   /* Incrementa valor no display */
	    }
}

#pragma vector=PORT1_VECTOR
__interrupt void PORT1_ISR(void)
{
    if (TST_BIT(P1IFG,BUTTON_1))                /* Se botao 1 for pressionado reseta a contagem */
        {
            display_number = 0x0F;
            CLR_BIT(P1IFG,BUTTON_1);
        }
    else if (TST_BIT(P1IFG,BUTTON_2))           /* Se o botao 2 for pressionado retorna true */
        {
            boolean = 1;
            CLR_BIT(P1IFG,BUTTON_2);
        }
    else if (TST_BIT(P1IFG,BUTTON_3))           /* Se o botao 3 for pressionado retorna false */
        {
            boolean = 0;
            CLR_BIT(P1IFG,BUTTON_3);
        }
}
