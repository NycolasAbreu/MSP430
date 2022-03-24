#ifndef __MSP430FR2355__
    #error "Clock system not supported for this device"
#endif

#include <msp430.h>
#include <stdint.h>

void init_clock_system(void) {

    // Configure two FRAM wait state as required by the device data sheet for MCLK
    // operation at 24MHz(beyond 8MHz) _before_ configuring the clock system.
    FRCTL0 = FRCTLPW | NWAITS_2 ;

    P2SEL1 |= BIT6 | BIT7;                       // P2.6~P2.7: crystal pins
    do
    {
        CSCTL7 &= ~(XT1OFFG | DCOFFG);           // Clear XT1 and DCO fault flag
        SFRIFG1 &= ~OFIFG;
    } while (SFRIFG1 & OFIFG);                   // Test oscillator fault flag

    __bis_SR_register(SCG0);                     // disable FLL
    CSCTL3 |= SELREF__XT1CLK;                    // Set XT1 as FLL reference source
    CSCTL0 = 0;                                  // clear DCO and MOD registers
    CSCTL1 = DCORSEL_7;                          // Set DCO = 24MHz
    CSCTL2 = FLLD_0 + 731;                       // DCOCLKDIV = 327358*731 / 1
    __delay_cycles(3);
    __bic_SR_register(SCG0);                     // enable FLL
    while(CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1));   // FLL locked

    /* CSCTL4 = SELMS__DCOCLKDIV | SELA__REFOCLK;
     * set XT1 (~32768Hz) as ACLK source, ACLK = 32768Hz
     * default DCOCLKDIV as MCLK and SMCLK source
     - Selects the ACLK source.
     * 00b = XT1CLK with divider (must be no more than 40 kHz)
     * 01b = REFO (internal 32-kHz clock source)
     * 10b = VLO (internal 10-kHz clock source) (1) <<< USANDO ESSE   */
    CSCTL4 = SELMS__DCOCLKDIV | SELA__REFOCLK;
}

/**
 * @brief  Configura temporizador A1 com contagem up e down.
 *
 * @param  none
 *
 * @retval none
 */
void config_timerB_1_as_pwm(){

    /* Estamos usando TB1CCR0 para contagem m�xima
     * que permite controle preciso sobre o per�odo
     * � poss�vel usar o overflow */

    /* Configura��o dos comparadores como PWM:
     *
     * TB1CCR0: Timer1_A Capture/Compare 0: per�odo do PWM
     *
     * OUTMOD_2: PWM output mode: 2 - PWM toggle/reset
     *
     * TB1CCR1 PWM duty cycle: TB1CCR1 / TB1CCR0 *
     * TB1CCR1 PWM duty cycle: TB1CCR2 / TB1CCR0 */

    /* Calculo do overflow comparador 0
     *
     *  - Periodo da frequencia escolhida
     *  T = 1 / 10kHz = 0.0001
     *  - Multiplique pela frequencia do timer
     *  Resolu��omax = T * 24MHz = 2400
     *  - Como � Up/Down devemos divider por 2
     *  Resolu��oUpDown = 2400 / 2 = 1200
    */

    TB1CCR0 = 1200-1;

    TB1CCTL1 = OUTMOD_2;
    TB1CCTL2 = OUTMOD_2;

    TB1CCR1 = 599;
    TB1CCR2 = 256;

    /*      .
    *      /|\                  +                < -Comparador 0: (m�ximo da contagem) -> per�odo do PWM
    *       |                 +   +
    *       |               +       +
    *       |-------------+---------- +          <--  Comparadores 1 e 2: raz�o c�clica
    *       |           +  |         | +
    *       |         +    |         |   +
    *       |       +      |         |     +
    *       |     +        |         |       +
    *       |   +          |         |         +
    *       | +            |         |           +
    * Timer +--------------|---- ----|-------------->
    *       |              |
    *       |
    *
    *       |--------------+         |--------------
    * Sa�da |              |         |
    *       +---------------++++++++++------------->
    */

    /* TBSSEL_2 -> Timer B clock source select: SMCLK
     * MC_3     -> Up/down mode: Timer counts up to TBxCL0 and down to 0000h
     * ID_3     -> Timer A input divider: divide by 4
     *
     * Configura��o da fonte do clock do timer 1 */
    TB1CTL = TBSSEL_2 | MC_3 | ID_0;
}


void main(void)
{
    /* Para o watchdog timer
     * Necess�rio para c�digo em depura��o */
    WDTCTL = WDTPW | WDTHOLD;

#if defined (__MSP430FR2355__)
    /* Disable the GPIO power-on default high-impedance mode */
    PM5CTL0 &= ~LOCKLPM5;
#endif

    init_clock_system();
    config_timerB_1_as_pwm();

    /* Liga��o f�sicas do timer nas portas
     * TB1.1 � o P2.0
     * TB1.2 � o P2.1
     */

    P2DIR = BIT0 | BIT1;

    /* Fun��o alternativa: liga��o dos pinos no temporizador
     * P2.0 -> ATIVA TB1.1
     * P2.1 -> ATIVA TB1.2
     */

    P2SEL0 = BIT0 | BIT1;

    while(1)
    {
        __bis_SR_register(LPM0_bits + GIE);
        /* C�digo de baixa prioridade da aplica��o */
    }
}
