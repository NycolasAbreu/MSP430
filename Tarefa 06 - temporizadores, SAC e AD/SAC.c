/*
 * SAC.c
 *
 *  Created on: 29 de jul de 2021
 *      Author: Nycolas
 */

#include "SAC.h"

/*
                        |+
      P1.3/OA0+   --(+) |   +
                        |      +---  P1.1/OA0O
      P1.2/OA0-   --(-) |   +
                        |+

                        |+
      P3.3/OA2+   --(+) |   +
                        |      +---  P3.1/OA2O
      P3.2/OA2-   --(-) |   +
                        |+
*/

void init_SAC(void)
{
    P3SEL0 |= BIT1 + BIT2 + BIT3;                            // Troca funcionamento das portas para abilitar SAC2
    P3SEL1 |= BIT1 + BIT2 + BIT3;

    //#### Configura SAC0 para BUFFER e entrada + em SAC2 ####
    SAC0OA |= NMUXEN + PMUXEN + NSEL_1 + OAPM + PSEL_2;
    // NMUXEN + PMUXEN = Enable negative and positive input
    // NSEL_1          = Select PGA source as OA negative input
    // OAPM            = Select low speed and low power mode
    // PSEL_2          = Paired OA source select
    SAC0PGA |= MSEL_1;                                  // Set as Unity-Gain Buffer Mode
    SAC0OA |= SACEN + OAEN;                             // Enable SAC and OA

    //#### Configura SAC2 para receber dados do sensor fora da placa ####
    SAC2OA |= NMUXEN + PMUXEN + NSEL_0 + OAPM + PSEL_0;
    // NMUXEN + PMUXEN = Enable negative and positive input
    // NSEL_0          = Select inverting pin as external source
    // OAPM            = Select low speed and low power mode
    // PSEL_0          = Select non-inverting pin as external source

    SAC2OA |= SACEN + OAEN;                             // Enable SAC and OA
}
