/*
 * main.c
 *
 *  Created on: 20 de ago de 2021
 *      Author: Nycolas C. de Abreu
 *      Instituto Federal de Santa Catarina
 *
 *      - Clock da CPU em 24MHZ
 *      - Transmiss�o e Recep��o de dados via UART
 *      - com taxa de transmiss�o de 19200bps
 *      - Utiliza��o de um PWM com 10khz
 *      - Utiliza��o do combo SAC para o sensor de luz
 *      - CPU � desligado at� o recebimento dos dados
 *
 *      - Comandos:
 *          -AT?                - Checa a comunica��o
 *          -AT+VERSION?        - Envia a vers�o do firmware
 *          -AT+CMODE=<param>   - Troca PWM entre bot�o e serial
 *          -AT+CCR=<param>     - Altera DutyCycle pelo serial
 *          -AT+SP=<param>      - Altera o par�metro de acionamento da luz
 *          -AT+LUZ?            - Envia o valor da luminosidade no momento
 *          -AT+INFO            - Envia o valor da luminosidade a cada 2seg (N�o implementado)
 *
 *                    MSP430FR2355
 *                 -----------------
 *             /|\|              XIN|-
 *              | |                 |
 *              --|RST          XOUT|-
 *                |                 |
 *                |    P4.3/UCA1TXD | --> TX
 *                |    P4.2/UCA1RXD | <-- RX
 *                |                 |
 * LED1 LIGHT <-- | P1.0       P2.0 | --> PWM
 *  LED2 COMM <-- | P6.6            |
 *    BUTTON1+<-- | P2.3            |
 *    BUTTON2-<-- | P4.1            |
 */

#include <msp430.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "uart_fr2355.h"
#include "string_handle.h"
#include "bits.h"
#include "CLK.h"
#include "SAC.h"
#include "ADC.h"
#include "PWM.h"

#define BUTTON1 BIT3
#define BUTTON2 BIT1

#ifndef __MSP430FR2355__
#error "Clock system not supported/tested for this device"
#endif

int main()
{
    char my_data[16];               //Buffer para armazenar mensagem recebida
    char buff[4];
    const char message[] = "OK";
    const char message1[] = "ALFA0.1";
    const char message2[] = "FAIL";
    const char message3[] = "OK-BUTTON";
    const char message4[] = "OK-SERIAL";

    volatile int convert;

    WDTCTL = WDTPW + WDTHOLD;       //Desliga Watchdog

    PM5CTL0 &= ~LOCKLPM5;           //Disable the GPIO power-on default high-impedance mode

    /* Inicializa hardware */
    init_CLK();                     //Inicia o clock com 24Mhz
    init_uart();                    //Inicia a comunica��o UART
    config_timerB_1_as_pwm();       //Liga��o f�sica do timer pwm (TB1.1 � o P2.0)
    config_timerB_0_debouncer();    //Inicia o timer TB0 como debouncer
    init_SAC();                     //Inicia SAC para m�dulo de luz
    init_ADC();                     //Inicia ADC para converter informa��o da luz

    P2DIR = BIT0;                   //Liga��o P2.0 (TIMER TB1.1) como sa�da
    P2SEL0 = BIT0;                  //Liga fun��o alternativa do pino de sa�da para timer ativando TB1.1

    P2REN |= BUTTON1;               //Resistores de pull up para bot�es
    P4REN |= BUTTON2;
    P2OUT |= BUTTON1;
    P4OUT |= BUTTON2;

    P1DIR |= BIT0;                  //Leds de depura��o
    P6DIR |= BIT6;

    while (1)
    {
        clearstring(my_data);

        __bis_SR_register(CPUOFF | GIE);                                        //Desliga a CPU enquanto pacote n�o chega

        uart_receive_package((uint8_t*) my_data, 16);                           //Configura o recebimento de um pacote de 4 bytes
        CPL_BIT(P6OUT, BIT6);

        if (strstr(my_data, "AT?") != NULL)                                     //<---Checa conex�o
            uart_send_package((uint8_t*) message, sizeof(message) - 1);

        else if (strstr(my_data, "AT+VERSION?") != NULL)                        //<---Retorna vers�o
            uart_send_package((uint8_t*) message1, sizeof(message1) - 1);

        else if (strstr(my_data, "AT+CMODE=") != NULL)                          //<---Alterna modo de bot�es e serial para pwm
        {
            convert = atoi(&my_data[9]);
            if (convert == 0)                                                   //Se for 0 habilita bot�es e desabilita serial
            {
                cmode = 0;
                uart_send_package((uint8_t*) message3, sizeof(message3) - 1);
            }
            else if (convert == 1)                                              //Se for 1 habilita serial e desabilita bot�es
            {
                cmode = 1;
                uart_send_package((uint8_t*) message4, sizeof(message4) - 1);
            }
            else                                                                //Qualquer outro valor � erro
                uart_send_package((uint8_t*) message2, sizeof(message2) - 1);
        }

        else if (strstr(my_data, "AT+CCR=") != NULL)                            //<---Altera o duty cycle do pwm pelo serial
        {
            convert = atoi(&my_data[7]);
            if (convert > 1199 || convert < 1 || cmode == 0)                    //Checa se est� dentro do permitido ou se est� desativado
                uart_send_package((uint8_t*) message2, sizeof(message2) - 1);
            else
            {
                change_pwm_pulse(convert);                                      //Troca o duty cycle pelo recebido no serial
                uart_send_package((uint8_t*) message, sizeof(message) - 1);
            }
        }

        else if (strstr(my_data, "AT+SP=") != NULL)                             //<---Altera o par�metro de acionamento da luz
        {
            convert = atoi(&my_data[6]);
            if (convert > 255 || convert < 1)
                uart_send_package((uint8_t*) message2, sizeof(message2) - 1);
            else
            {
                change_light_param(convert);                                    //Troca o par�metro de acionamento da luz pelo recebido no serial
                uart_send_package((uint8_t*) message, sizeof(message) - 1);
            }

        }

        else if (strstr(my_data, "AT+LUZ?") != NULL)                            //<---Retorna o valor da luminosidade atual
        {
            sprintf(buff, "%d", return_light_value());                          //Fazendo a mudan�a de int para char
            uart_send_package((uint8_t*) buff, sizeof(buff) - 1);
        }

        else if (strstr(my_data, "AT+INFO") != NULL)
        {
            uart_send_package((uint8_t*) message, sizeof(message) - 1);
            // falta c�digo
        }
    }
}
