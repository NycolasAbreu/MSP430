#ifndef __MSP430FR2355__
    #error "Clock system not supported for this device"
#endif

#include <msp430.h>
#include "gpio.h"

#define LED BIT0        // Define o led da placa de desenvolvimento que � P1.0
#define LED_PORT P1

/**
  * @brief  Configura temporizador watchdog.
  *
  * @param  none
  *
  * @retval none
  */
void config_wd_as_timer(){
    /* Configura Watch dog como temporizador:
     *-
     * WDT_ADLY_1000 <= (WDTPW+WDTTMSEL+WDTCNTCL+WDTIS2+WDTSSEL0)
     *
     * WDTPW -> "Senha" para alterar confgiura��o.
     * WDTTMSEL -> Temporizador ao inv�s de reset.
     * WDTSSEL -> Fonte de clock de ACLK
     * WDTIS2 -> Per�odo de 1000ms com ACLK = 32.768Hz
     *
     */
    WDTCTL = WDT_ADLY_1000;     // WDTCTL = Watchdog Timer Control Register (CONTROLA O TIMER DO WDT)
                                /*
                                 WDT-interval times [1ms] coded with Bits 0-2
                                 WDT is clocked by fSMCLK (assumed 1MHz)
                                #define WDT_MDLY_32             (WDTPW+WDTTMSEL+WDTCNTCL+WDTIS2)                         32ms interval (default)
                                #define WDT_MDLY_8              (WDTPW+WDTTMSEL+WDTCNTCL+WDTIS2+WDTIS0)                  8ms
                                #define WDT_MDLY_0_5            (WDTPW+WDTTMSEL+WDTCNTCL+WDTIS2+WDTIS1)                  0.5ms
                                #define WDT_MDLY_0_064          (WDTPW+WDTTMSEL+WDTCNTCL+WDTIS2+WDTIS1+WDTIS0)           0.064ms
                                 WDT is clocked by fACLK (assumed 32KHz)
                           >>>  #define WDT_ADLY_1000           (WDTPW+WDTTMSEL+WDTCNTCL+WDTIS2+WDTSSEL0)                1000ms
                                #define WDT_ADLY_250            (WDTPW+WDTTMSEL+WDTCNTCL+WDTIS2+WDTSSEL0+WDTIS0)         250ms
                                #define WDT_ADLY_16             (WDTPW+WDTTMSEL+WDTCNTCL+WDTIS2+WDTSSEL0+WDTIS1)         16ms
                                #define WDT_ADLY_1_9            (WDTPW+WDTTMSEL+WDTCNTCL+WDTIS2+WDTSSEL0+WDTIS1+WDTIS0)  1.9ms
                                 Watchdog mode -> reset after expired time
                                 WDT is clocked by fSMCLK (assumed 1MHz)
                                #define WDT_MRST_32             (WDTPW+WDTCNTCL+WDTIS2)                                  32ms interval (default)
                                #define WDT_MRST_8              (WDTPW+WDTCNTCL+WDTIS2+WDTIS0)                           8ms
                                #define WDT_MRST_0_5            (WDTPW+WDTCNTCL+WDTIS2+WDTIS1)                           0.5ms
                                #define WDT_MRST_0_064          (WDTPW+WDTCNTCL+WDTIS2+WDTIS1+WDTIS0)                    0.064ms
                                 WDT is clocked by fACLK (assumed 32KHz)
                                #define WDT_ARST_1000           (WDTPW+WDTCNTCL+WDTSSEL0+WDTIS2)                         1000ms
                                #define WDT_ARST_250            (WDTPW+WDTCNTCL+WDTSSEL0+WDTIS2+WDTIS0)                  250ms
                                #define WDT_ARST_16             (WDTPW+WDTCNTCL+WDTSSEL0+WDTIS2+WDTIS1)                  16ms
                                #define WDT_ARST_1_9            (WDTPW+WDTCNTCL+WDTSSEL0+WDTIS2+WDTIS1+WDTIS0)           1.9ms
                                */

    SFRIE1 |= WDTIE;        /* Ativa IRQ do Watchdog */
}


int main(void)
{
    /* Desliga watchdog imediatamente */
    WDTCTL = WDTPW | WDTHOLD;

#if defined (__MSP430FR2355__)
    /* Disable the GPIO power-on default high-impedance mode */
    PM5CTL0 &= ~LOCKLPM5;
#endif

    /* Configura��es de hardware */
    config_wd_as_timer();

    PORT_DIR(LED_PORT) = BIT0;      // P1DIR = BIT0  LED COMO SAIDA

    /* Entra em modo de economia de energia */
    __bis_SR_register(LPM0_bits + GIE);

}


/* ISR do watchdog: executado toda a vez que o temporizador estoura */
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(WDT_VECTOR))) watchdog_timer (void)
#else
#error Compiler not supported!
#endif
{
    PORT_OUT(LED_PORT) ^= LED;
}
