#include <msp430.h>
#include "led_display.h"

/* Tabela de convers�o em flash: Anodo comum */
#ifdef COM_ANODO
const uint8_t convTable[] = {0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x18, 0x08, 0x03, 0x46, 0x21, 0x06, 0x0E};
#endif

void display_init() {

    /* Configura��o de portas */
    DISPLAY_PORT_DIR = 0xff;
    DISPLAY_PORT_OUT = 0;

}

void display_write(uint8_t data){

    /* Data n�o pode ser maior que 0x0f */
    data = data & 0x0f;
    /* Escreve no display */
    DISPLAY_PORT_OUT = convTable[data];

}
