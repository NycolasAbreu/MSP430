/*2. Implementar e um programam que liga um LED caso um botao esteja
acionado (bot�ao �e mantido acionado).
 *
 */
//Definir as variaveis
#define LED_1 BIT0
#define BUTTON BIT3


#include <msp430.h>
#include "bits.h"

void hardware_init()
{
    P1DIR |= LED_1;// Pino P1.0 como saida

    //Configurando o botao
    P1REN = BUTTON;
    //Configurando em pull up
    P1OUT = BUTTON;

}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

     /* Incializa o hardware */
     hardware_init();

     //P1IN -> Leitura de dados quando configurado como entrada
     while(1){

            if (TST_BIT(P1IN, BUTTON))    /*Ve se o botao esta pressionado, se estiver, retorna falso */
                CLR_BIT(P1OUT, LED_1);    /* Equivalente a P1OUT &= ~(BIT0 | BIT6); */
            else  /*Liga o led*/
                SET_BIT(P1OUT, LED_1);    /* Equivalente a: P1OUT |= BIT0 | BIT6; */
        }

    return 0;
}
