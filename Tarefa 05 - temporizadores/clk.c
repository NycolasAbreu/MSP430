/*
 * clk.c
 *
 *  Created on: 21 de jul de 2021
 *      Author: Nycolas
 */

#include <msp430.h>
#include "clk.h"

/**
  * @brief  Configura Clock do sistema (SMCLK) para funcionar no máximo
  *         permitido 24MHz.
  * @param  none
  * @retval none
  */
void init_clock_system(void)
{

  // Configure two FRAM wait state as required by the device data sheet for MCLK
  // operation at 24MHz(beyond 8MHz) _before_ configuring the clock system.
  FRCTL0 = FRCTLPW | NWAITS_2;

  P2SEL1 |= BIT6 | BIT7; // P2.6~P2.7: crystal pins
  do
  {
    CSCTL7 &= ~(XT1OFFG | DCOFFG); // Clear XT1 and DCO fault flag
    SFRIFG1 &= ~OFIFG;
  } while (SFRIFG1 & OFIFG); // Test oscillator fault flag

  __bis_SR_register(SCG0);  // disable FLL
  CSCTL3 |= SELREF__XT1CLK; // Set XT1 as FLL reference source
  CSCTL0 = 0;               // clear DCO and MOD registers
  CSCTL1 = DCORSEL_7;       // Set DCO = 24MHz
  CSCTL2 = FLLD_0 + 731;    // DCOCLKDIV = 327358*731 / 1
  __delay_cycles(3);
  __bic_SR_register(SCG0);  // enable FLL
  while (CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1))
    ; // FLL locked

  /* CSCTL4 = SELMS__DCOCLKDIV | SELA__REFOCLK;
     * set XT1 (~32768Hz) as ACLK source, ACLK = 32768Hz
     * default DCOCLKDIV as MCLK and SMCLK source
     - Selects the ACLK source.
     * 01b = REFO (internal 32-kHz clock source) <<< USANDO ESSE
     */
  CSCTL4 = SELMS__DCOCLKDIV | SELA__REFOCLK;
}
