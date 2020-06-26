/*
 * softPWM.c
 *
 *  Created on: Dec 14, 2018
 *      Author: dsolano
 */

#include <pwm_lpc1788.h>

void pwm_pin_init(void){
	Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 7,  (IOCON_FUNC3 | IOCON_MODE_INACT)); //PWM 0,6
	Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 11, (IOCON_FUNC3 | IOCON_MODE_INACT)); //PWM 0,7
}

void PWM_Init(PWM_ID_t pwmid){
	pwm_pin_init();

	if (pwmid == 0){
		Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_PWM0);
	}else{
		Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_PWM1);
	}
}


