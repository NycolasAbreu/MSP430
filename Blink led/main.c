#include <msp430.h> 

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	P1DIR |= BIT0 | BIT1; // Pino P1.0 e P1.1 como saida

	while(1){
	    P1OUT|=BIT0 | BIT1;     //  Liga LED 1 e 2 SET_BIT
	    __delay_cycles(5000);
	    P1OUT&=~(BIT0|BIT1);    //  Desliga LED CLR_BIT
	    __delay_cycles(5000);
	}

	return 0;
}
