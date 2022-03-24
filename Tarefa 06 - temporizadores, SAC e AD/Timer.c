/*
 * Timer.c
 *
 *  Created on: 29 de jul de 2021
 *      Author: Nycolas
 */

#include "Timer.h"

/**
  * @brief  Configura temporizador B1 para trigger do ADC
  *
  * @param  none
  *
  * @retval none
  */
void init_TIMERB1(void)
{
    TB1CCR0 = 200 - 1;                      // Contagem máxima do timer 1 Frequencia de 5 MHz com SMCLK em 1 MHz

    TB1CCTL1 = OUTMOD_7;                    // Habilita saída interna do do comparador 0: CCR1 reset/set

    TB1CCR1 = 100;                          // Valor de comparação 1: deve ser menor que TB1CCR0

    TB1CTL = TBSSEL_2 | MC_2 | TBCLR;
    // TBSSEL_2 = SMCLK como clock source
    // MC_2     = modo de contagem contínua
    // TBCLR    = limpa registrador de contagem
}
