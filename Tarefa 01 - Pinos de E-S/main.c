/*  main.c
 *
 *      Created on: Mai 30, 2021
 *      Author: Nycolas Coelho de Abreu
 *      Instituto Federal de Santa Catarina
 *
 *      Contador hexadecimal com reset
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
#include <stdint.h>                         /* Tipos uint16_t, uint8_t, ... */
#include "led_display.h"
#include "bits.h"

#define BUTTON_1 BIT0
#define BUTTON_2 BIT1
#define BUTTON_3 BIT2
#define DELAY_FAST 10000
#define DELAY_SLOW 100000

void main(void)
{
    uint8_t display_number = 0;             /* Incremento do display */
    uint8_t boolean = 0;                    /* Boleano para delay */

    WDTCTL = WDTPW | WDTHOLD;               /* Para o watchdog timer */

    P1REN = BUTTON_1 | BUTTON_2 | BUTTON_3; /* Configurando em pull up */
    P1OUT = BUTTON_1 | BUTTON_2 | BUTTON_3;

    display_init();                         /* Inicializa display modo anodo */

    while(1)
    {
        if (!TST_BIT(P1IN, BUTTON_2))       /* Ve se o botao esta pressionado, se estiver, retorna true */
            boolean = 1;
        else if (!TST_BIT(P1IN, BUTTON_3))
            boolean = 0;
        else if (!TST_BIT(P1IN, BUTTON_1))  /* Se botao 1 estiver pressionado, Reseta a contagem */
            display_number = 0;

        display_write(display_number);

        if (boolean)                        /* Se 1 então delay rapido Se 0 então delay devagar */
            __delay_cycles(DELAY_FAST);
        else
            __delay_cycles(DELAY_SLOW);

        display_number++;                   /* Incrementa e limita valor de x até 0x0f */
        display_number = display_number & 0x0f;
    }
}
