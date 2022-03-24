/*  main.c
 *
 *      Created on: Jun 13, 2021
 *      Author: Nycolas Coelho de Abreu
 *      Instituto Federal de Santa Catarina
 *
 *      Contador de pessoas com sensor de entrada
 *      e saída, utilizando LCD 16x2 para interface da contagem
 *      - Botão 1: para reset.
 *      - Botão 2: incrementa o valor de pessoas que entraram.
 *      - Botão 3: incrementa o valor de pessoas que saíram.
 *
 *                  MSP430G2553
 *               -----------------
 *           /|\|              XIN|-
 *            | |                 |
 *            --|RST          XOUT|-
 *              |                 |
 *  RST/LCD <-- | P1.2            |
 *    E/LCD <-- | P1.3            |
 *              |                 |
 *      LCD <-- | P2.[0..3]       |
 *  BUTTON1 <-- | P2.5            |
 *  BUTTON2 <-- | P2.6            |
 *  BUTTON3 <-- | P2.7            |
 *
 */


#include <msp430.h>
#include <stdio.h>

#include "gpio.h"
#include "lcd.h"

#define BUTTON_PORT P2
#define BUTTON_1  BIT5
#define BUTTON_2  BIT6
#define BUTTON_3  BIT7
#define MAX 12

volatile uint8_t chegada = 0;
volatile uint8_t saida = 0;
volatile uint8_t total = 0;

void config_ext_irq(){

    PORT_REN(BUTTON_PORT) = BUTTON_1 | BUTTON_2 | BUTTON_3;       /* Pull up */
    PORT_OUT(BUTTON_PORT) = BUTTON_1 | BUTTON_2 | BUTTON_3;

    PORT_IE(BUTTON_PORT) =  BUTTON_1 | BUTTON_2 | BUTTON_3;       /* Habilitação da IRQ apenas botão */

    PORT_IES(BUTTON_PORT) = BUTTON_1 | BUTTON_2 | BUTTON_3;       /* Transição de nível alto para baixo */

    CLR_BIT(PORT_IFG(BUTTON_PORT),(BUTTON_1 | BUTTON_2 | BUTTON_3));/* Limpa alguma IRQ pendente */
}

void main(){
    char string[8];
    //char teste[8];

    /* Configuração de hardware */
    WDTCTL = WDTPW | WDTHOLD;

#if defined (__MSP430FR2355__)
    /* Disable the GPIO power-on default high-impedance mode */
    PM5CTL0 &= ~LOCKLPM5;
#endif

    /* Inicializa hardare: veja lcd.h para
     * configurar pinos */
    lcd_init_4bits();
    /* Escreve string */

    /* Configura interupções */
    config_ext_irq();

    /* Habilita IRQs e desliga CPU */
    __bis_SR_register(GIE);

    lcd_send_data(LCD_LINE_1, LCD_CMD);
    lcd_write_string("Cont. de pessoas");
    lcd_send_data(LCD_LINE_0, LCD_CMD);
    lcd_write_string("Total:");
    lcd_send_data(LCD_LINE_0+8, LCD_CMD);
    lcd_write_string("E:");
    lcd_send_data(LCD_LINE_0+12, LCD_CMD);
    lcd_write_string("S:");

    _delay_cycles(100000);

    while (1){

        lcd_send_data(LCD_LINE_0+6, LCD_CMD);
        snprintf(string, 8, "%d", total);
        lcd_write_string(string);

        if (total == 0)
        {
            lcd_send_data(LCD_LINE_0+7, LCD_CMD);
            lcd_write_string(" ");
        }

        lcd_send_data(LCD_LINE_0+10, LCD_CMD);
        snprintf(string, 8, "%d", chegada);
        lcd_write_string(string);

        if(chegada == MAX)
        {
            int i;
            for( i=0 ; i < 10 ; i++){
                lcd_send_data(LCD_LINE_1, LCD_CMD);
                lcd_write_string("Máx. de pessoas");
                lcd_send_data(LCD_LINE_1, LCD_CMD);
                lcd_write_string("                ");
            }
            lcd_send_data(LCD_LINE_1, LCD_CMD);
            lcd_write_string("Cont. de pessoas");
        }

        if (chegada == 9 | chegada == 0)
        {
            lcd_send_data(LCD_LINE_0+11, LCD_CMD);
            lcd_write_string(" ");
        }

        lcd_send_data(LCD_LINE_0+14, LCD_CMD);
        snprintf(string, 8, "%d", saida);
        lcd_write_string(string);

        if (saida == 0)
        {
            lcd_send_data(LCD_LINE_0+15, LCD_CMD);
            lcd_write_string(" ");
        }

        _delay_cycles(10000);
    }
}

/* Port 2 ISR (interrupt service routine) */
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT1_VECTOR))) Port_2 (void)
#else
#error Compiler not supported!
#endif
{
    if (TST_BIT(PORT_IFG(BUTTON_PORT),BUTTON_1))                /* Se botao 1 for pressionado reseta a contagem */
    {
        chegada = 0;
        total = 0;
        saida = 0;

        CLR_BIT(PORT_IFG(BUTTON_PORT),BUTTON_1);
    }
    else if (TST_BIT(PORT_IFG(BUTTON_PORT),BUTTON_2))           /* Se o botao 2 for pressionado aumenta a quantidade */
    {
        if (chegada < MAX)
        {
            total++;
            chegada++;
        }
        CLR_BIT(PORT_IFG(BUTTON_PORT),BUTTON_2);
    }
    else if (TST_BIT(PORT_IFG(BUTTON_PORT),BUTTON_3))           /* Se o botao 3 for pressionado diminui a quantidade */
    {
        if (chegada > 0)
        {
            saida++;
            chegada--;
        }
        CLR_BIT(PORT_IFG(BUTTON_PORT),BUTTON_3);
    }
}
