/*
 * 02_main_display_7_seg.c
 *
 *  Created on: Feb 7, 2020
 *      Author: Renan Augusto Starke
 *      Instituto Federal de Santa Catarina
 */

#include <msp430.h>

/* Tipos uint16_t, uint8_t, ... */
#include <stdint.h>

#include "led_display.h"

void main(void)
{
    /* Para o watchdog timer
     * Necessário para código em depuração */
    WDTCTL = WDTPW | WDTHOLD;

    uint8_t x = 0;

    /* Inicializa displays */
    display_init();

    while(1)
    {
        display_write(x);

        /* Delay */
        __delay_cycles(50000);

        /* Incrementa e limita valor de x até 0x0f */
        x++;
        x = x & 0x0f;
    }
}
