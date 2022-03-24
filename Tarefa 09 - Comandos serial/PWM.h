/*
 * PWM.h
 *
 *  Created on: 21 de jul de 2021
 *      Author: Nycolas
 */

#ifndef PWM_H_
#define PWM_H_

#include <msp430.h>
#include <stdint.h>
#include "gpio.h"
#include "bits.h"

volatile int cmode;
void config_timerB_1_as_pwm(void);
void config_timerB_0_debouncer(void);
void change_pwm_pulse(volatile int);

#endif /* PWM_H_ */
