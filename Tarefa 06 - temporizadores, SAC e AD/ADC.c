/*
 * ADC.c
 *
 *  Created on: 29 de jul de 2021
 *      Author: Nycolas
 */

#include "ADC.h"

void init_ADC(void)
{
    ADCCTL0 |= ADCSHT_2 | ADCON;
    // ADCSHT_2 = ADC sample-and-hold time to 16 ADCCLK cycles
    // ADCON    = ADC on

    ADCCTL1 |= ADCSHP | ADCSHS_2 | ADCCONSEQ_2 | ADCSSEL_0;
    // ADCSHP      = Signal is sourced from the sampling timer
    // ADCSHS_2    = Timer trigger to TB1.1B (LIGADO INTERNAMENTE)
    // ADCCONSEQ_2 = Conversion sequence Repeat-single-channel
    // ADCSSEL_0   = ADC clock MODCLK

    ADCCTL2 &= ~ADCRES;                             /* 8-bit conversion results */

    ADCMCTL0 |= ADCINCH_1 | ADCSREF_1;
    // ADCINCH_1 = Input channel select (ESTA CONECTADO NA SAIDA DO SAC BUFFER OA0O)
    // ADCSREF_0 = Vref=3.3V

    ADCIE |= ADCIE0;                                /* Enable ADC ISQ */

    /* Configure reference interna  */
    PMMCTL0_H = PMMPW_H;                            // Unlock the PMM registers
    PMMCTL2 |= INTREFEN;                            // Enable internal reference
    __delay_cycles(400);                            // Delay for reference settling

    ADCCTL0 |= ADCENC;                              /* Enable ADC */

    /* TB1CTL |= TBCLR;  */                         /* Limpar timer para maior sincronismo */
}

/* ISR do ADC12. Executada quando a conversão terminar */
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=ADC_VECTOR
__interrupt void ADC_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ADC_VECTOR))) ADC_ISR (void)
#else
#error Compiler not supported!
#endif
{
    //P1OUT ^= BIT0;  /* Debug: Piscar quando executar a ISR */

    switch(__even_in_range(ADCIV,ADCIV_ADCIFG))
    {
    case ADCIV_NONE:
        break;
    case ADCIV_ADCOVIFG:
        break;
    case ADCIV_ADCTOVIFG:
        break;
    case ADCIV_ADCHIIFG:
        break;
    case ADCIV_ADCLOIFG:
        break;
    case ADCIV_ADCINIFG:
        break;
    case ADCIV_ADCIFG:
        /* Copia dados e acorda o main */
        ADC_Result = ADCMEM0;

        if (ADC_Result < 0x032)
            P6OUT &= ~BIT6;
        else
            P6OUT |= BIT6;

        __bic_SR_register_on_exit(LPM0_bits);
        break;
    default:
        break;
    }
}
