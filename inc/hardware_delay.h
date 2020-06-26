/*
 * systick_delay.h
 *
 *  Created on: Nov 4, 2014
 *      Author: dsolano
 */

#ifndef SYSTICK_DELAY_H_
#define SYSTICK_DELAY_H_
#ifdef __cplusplus
extern "C"
{
#endif


#include <chip.h>
#include <lpc_types.h>

#define dly_1us			1
#define dly_5us			5
#define dly_10us		10
#define dly_100us		100
#define dly_1ms			1000
#define dly_5ms			5000
#define dly_10ms		10000
#define dly_100ms		100000
#define dly_half_sec	500000
#define dly_1sec		1000000



#define TIMER0			LPC_TIMER0
#define TIMER1			LPC_TIMER1


void _delay_uS (uint32_t delayTicks);
void _delay_ms (uint32_t delayTicks);
void initTimer0(uint32_t freq) ;
void startTimer0(void);
uint_fast32_t stopTimer0(void);
uint_fast32_t getTimer0_counter(void);
//void delayUS(unsigned int microseconds); //Using Timer0;
//void delayMS(unsigned int milliseconds); //Using Timer0

#ifdef __cplusplus
}
#endif

#endif /* SYSTICK_DELAY_H_ */
