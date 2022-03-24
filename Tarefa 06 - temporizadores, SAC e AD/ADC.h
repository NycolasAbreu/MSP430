/*
 * ADC.h
 *
 *  Created on: 29 de jul de 2021
 *      Author: Nycolas
 */

#ifndef ADC_H_
#define ADC_H_

#include <msp430.h>
#include <stdint.h>

void init_ADC(void);
volatile uint16_t ADC_Result;

#endif /* ADC_H_ */
