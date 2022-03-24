/*
 * WDT_as_timer.c
 *
 *  Created on: 10 de jul de 2021
 *      Author: Nycolas
 */

#include "WDT_as_timer.h"

volatile uint16_t contador;

void delay_ms(uint16_t tempo)
{
     contador = tempo << 1;               // 0,5ms * 2 = 1ms  (1 Shift pra esquerda é a mesma coisa que multiplicar por 2)
     WDTCTL = WDT_MDLY_0_5;               // 0,5ms WDTCTL = Watchdog Timer Control Register (CONTROLA O TIMER DO WDT)
     SFRIE1 |= WDTIE;                     // Ativa interrupção do Watchdog
     __bis_SR_register(LPM3_bits + GIE);     // Desliga a CPU e ativa as interrupções
}

// ISR do watchdog: executado toda a vez que o temporizador estoura
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(WDT_VECTOR))) watchdog_timer (void)
#else
#error Compiler not supported!
#endif
{
    static uint16_t ref = 0;              // Variavel static para lembrar do ultimo valor, e não igualar a 0 na proxima iteração

    ref++;                                // Referencia para contar os ticks
    if (ref == contador)                  // Se a referencia for igual ao tempo inserido então sai do modo sleep
    {
        ref = 0;                          // Para contar novamente na próxima chamada zeramos as variaveis
        contador = 0;
        __bic_SR_register_on_exit(LPM3_bits);// Acorda o CPU
    }
}
