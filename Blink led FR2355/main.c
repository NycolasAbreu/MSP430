#include <msp430.h> 

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;
    P1DIR |= BIT0; // Pino P1.0 como saida

    while(1){
        P1OUT |= BIT0;     //  Liga LED 1 e 2 SET_BIT
        __delay_cycles(100000);
        P1OUT&= ~BIT0;    //  Desliga LED CLR_BIT
        __delay_cycles(100000);
    }

    return 0;
}
