/*
 * uart_fr2355.c
 *
 *  Created on: Aug 19, 2021
 *      Author: Nycolas Abreu
 *      Instituto Federal de Santa Catarina
 *
 *      - Biblioteca de comunica��o UART.
 *      - Utiliza UCA1 em modo UART 19200 bps
 */

#include <msp430.h>
#include <stdint.h>

#include <uart_fr2355.h>

#ifndef __MSP430FR2355__
#error "Library no supported/validated in this device."
#endif

struct uart_status_t
{
    /* Estado de envio */
    uint8_t *data_to_send;
    uint8_t send_size;
    uint8_t send_busy;

    /* Estado de recep��o */
    uint8_t *data_to_receive;
    uint8_t receive_size;
    uint8_t receive_busy;
};

volatile struct uart_status_t uart_status = { 0 };

/**
 * @brief  Configura o hardware USCI0 para UART com baudrate em 19200.
 *         FLL em 24 MHz. Ajustar o baudrate conforme a frequ�ncia de
 *         opera��o da CPU
 *
 * @param  none
 *
 * @retval none
 */
void init_uart()
{
    /* Fun��o alternativa dos pinos:
     * - P4.2 = RXD
     * - P4.3 = TXD
     */
    P4SEL0 = BIT2 | BIT3;

#ifdef CLOCK_24MHz
    UCA1CTLW0 |= UCSWRST;
    /* Fonte de clock SMCLK */
    UCA1CTLW0 |= UCSSEL_2;

    /* Veja http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html */
    UCA1BR0 = 78;
    UCA1BR1 = 0;
    UCA1MCTLW = 0x00 | UCOS16;
#else
#error "Clock system not defined for UART support"
#endif

    /* Initicializa��o do eUSCI */
    UCA1CTLW0 &= ~UCSWRST;

    /* Habilita��o da ISR */
    UCA1IE |= UCRXIE | UCTXIE;
}

/**
 * @brief  Envia um pacote para o ESCI.
 *         Utiliza IRQ de transmiss�o para o envio dos bytes.
 *
 *         Use com IRS habilitadas.
 *
 * @param  data: endere�o inicial dos dados do pacote.
 *         size: tamanho do pacote.
 *
 * @retval none
 */
void uart_send_package(uint8_t *data, uint8_t size)
{

    /* Serializa a transmiss�o de v�rios pacotes */
    while (uart_status.send_busy != 0);

    /* Condi��es de estado */
    uart_status.data_to_send = data;
    uart_status.send_size = size - 1;
    uart_status.send_busy = 1;

    /* Envia primeiro byte */
    UCA1TXBUF = data[0];
}

/**
 * @brief  Recebe um pacote do ESCI.
 *         Utiliza IRQ de recebimento de cada byte.
 *
 *         Use com IRS habilitadas.
 *
 * @param  data: endere�o inicial dos dados do pacote.
 *         size: tamanho do pacote.
 *
 * @retval none
 */
void uart_receive_package(uint8_t *data, uint8_t size)
{

    /* Serializa a transmiss�o de v�rios pacotes */
    while (uart_status.receive_busy != 0);

    /* Condi��es de estado */
    uart_status.data_to_receive = data;
    uart_status.receive_size = size;
    uart_status.receive_busy = 1;
}

/* ISR da EUSCI: acontece quando algum byte � recebido, transmitido, etc
 * conforme habilita��es na inicializa��o  */
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_A1_VECTOR))) USCI_A1_ISR (void)
#else
#error Compiler not supported!
#endif
{
    volatile uint8_t data;

    switch (__even_in_range(UCA1IV, USCI_UART_UCTXCPTIFG))
    {
    case USCI_NONE:
        break;
    case USCI_UART_UCRXIFG: /* Received IRQ */
        data = UCA1RXBUF;

        if (uart_status.receive_busy)
        {
            /* Guarda dados */
            *(uart_status.data_to_receive++) = data;
            uart_status.receive_size--;

            /* Se todos os dados foram recebidos,
             * acorda main e desliga recep��o */
            if (uart_status.receive_size == 0 || data == '\n')
            {
                uart_status.receive_busy = 0;
                __bic_SR_register_on_exit(CPUOFF);
            }
        }
        break;

    case USCI_UART_UCTXIFG: /* Transmit IRQ */
        if (uart_status.send_size != 0)
        {
            UCA1TXBUF = *(++uart_status.data_to_send);
            uart_status.send_size--;
        }
        else
        {
            /* Condi��es de t�rmino de envio de pacote */
            /* Desbloqueia serializa��o de pacotes */
            uart_status.send_busy = 0;
            /* Limpa flag de final de transmiss�o pois n�o h�
             * mais nada a enviar          */
            __bic_SR_register_on_exit(CPUOFF);
        }
        break;

    case USCI_UART_UCSTTIFG: /*  START byte received interrupt. */
        break;
    case USCI_UART_UCTXCPTIFG: /* Transmit complete interrupt. */
        break;

    default:
        break;
    }
}
