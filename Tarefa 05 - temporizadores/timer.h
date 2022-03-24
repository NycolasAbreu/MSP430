/*
 * timer.h
 *
 *  Created on: 21 de jul de 2021
 *      Author: Nycolas
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <msp430.h>
#include <stdint.h>
#include "gpio.h"
#include "bits.h"

void config_timerB_1_as_pwm(void);
void config_timerB_0_debouncer(void);

#endif /* TIMER_H_ */
