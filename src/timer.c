/****************************************************************************
 *   $Id:: timer.c 5788 2010-12-02 01:13:25Z usb00423                       $
 *   Project: NXP LPC17xx Timer for PWM example
 *
 *   Description:
 *     This file contains timer code example which include timer 
 *     initialization, timer interrupt handler, and APIs for timer access.
 *
 ****************************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
****************************************************************************/
#include <chip.h>
#include "timer.h"

typedef struct{
	TimerCallFn Timer0_CallBack;
	TimerCallFn Timer1_CallBack;
	TimerCallFn Timer2_CallBack;
	TimerCallFn Timer3_CallBack;
}TimerCB_t;

TimerCB_t tcb;
uint32_t counter0, counter1, counter2, counter3;


/*****************************************************************************
** Function name:		delayMs
**
** Descriptions:		Start the timer delay in milo seconds
**						until elapsed
**
** parameters:			timer number, Delay value in milo second			 
** 						
** Returned value:		None
** 
*****************************************************************************/
void delayMs(uint8_t timer_num, uint32_t delayInMs)
{
  if ( timer_num == 0 )
  {
	LPC_TIMER0->TCR = 0x02;		/* reset timer */
	LPC_TIMER0->PR  = 0x00;		/* set prescaler to zero */
	LPC_TIMER0->MR[0] = delayInMs * (Chip_Clock_GetPeripheralClockRate() / 1000 - 1);
	LPC_TIMER0->IR  = 0xff;		/* reset all interrrupts */
	LPC_TIMER0->MCR = 0x04;		/* stop timer on match */
	LPC_TIMER0->TCR = 0x01;		/* start timer */
  
	/* wait until delay time has elapsed */
	while (LPC_TIMER0->TCR & 0x01);
  }
  else if ( timer_num == 1 )
  {
	LPC_TIMER1->TCR = 0x02;		/* reset timer */
	LPC_TIMER1->PR  = 0x00;		/* set prescaler to zero */
	LPC_TIMER1->MR[0] = delayInMs * (Chip_Clock_GetPeripheralClockRate()  / 1000 - 1);
	LPC_TIMER1->IR  = 0xff;		/* reset all interrrupts */
	LPC_TIMER1->MCR = 0x04;		/* stop timer on match */
	LPC_TIMER1->TCR = 0x01;		/* start timer */
  
	/* wait until delay time has elapsed */
	while (LPC_TIMER1->TCR & 0x01);
  }
  else if ( timer_num == 2 )
  {
	  LPC_TIMER2->TCR = 0x02;		/* reset timer */
	  LPC_TIMER2->PR  = 0x00;		/* set prescaler to zero */
	  LPC_TIMER2->MR[0] = delayInMs * (Chip_Clock_GetPeripheralClockRate()  / 1000 - 1);
	  LPC_TIMER2->IR  = 0xff;		/* reset all interrrupts */
	  LPC_TIMER2->MCR = 0x04;		/* stop timer on match */
	  LPC_TIMER2->TCR = 0x01;		/* start timer */
  
	/* wait until delay time has elapsed */
	while (LPC_TIMER2->TCR & 0x01);
  }
  else if ( timer_num == 3 )
  {
	LPC_TIMER3->TCR = 0x02;		/* reset timer */
	LPC_TIMER3->PR  = 0x00;		/* set prescaler to zero */
	LPC_TIMER3->MR[0] = delayInMs * (Chip_Clock_GetPeripheralClockRate()  / 1000 - 1);
	LPC_TIMER3->IR  = 0xff;		/* reset all interrrupts */
	LPC_TIMER3->MCR = 0x04;		/* stop timer on match */
	LPC_TIMER3->TCR = 0x01;		/* start timer */
  
	/* wait until delay time has elapsed */
	while (LPC_TIMER3->TCR & 0x01);
  }
  return;
}

/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void TIMER0_IRQHandler (void) 
{  
    LPC_TIMER0->IR = 1;		/* clear interrupt flag */
    counter0++;
  tcb.Timer0_CallBack(counter0);
  return;
}

/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void TIMER1_IRQHandler (void)  
{  
  LPC_TIMER1->IR = 1;		/* clear interrupt flag */
  counter1++;
  tcb.Timer1_CallBack(counter1);
  return;
}

/******************************************************************************
** Function name:		Timer2_IRQHandler
**
** Descriptions:		Timer/Counter 2 interrupt handler
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void TIMER2_IRQHandler (void)  
{  
  LPC_TIMER2->IR = 1;			/* clear interrupt flag */
  counter2++;
  tcb.Timer2_CallBack(counter2);
  return;
}

/******************************************************************************
** Function name:		Timer3_IRQHandler
**
** Descriptions:		Timer/Counter 3 interrupt handler
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void TIMER3_IRQHandler (void)  
{  
  LPC_TIMER3->IR = 1;			/* clear interrupt flag */
  counter3++;
  tcb.Timer3_CallBack(counter3);
  return;
}

/******************************************************************************
** Function name:		enable_timer
**
** Descriptions:		Enable timer
**
** parameters:			timer number: 0 or 1 or 2 or 3
** Returned value:		None
** 
******************************************************************************/
void enable_timer( uint8_t timer_num )
{
  if ( timer_num == 0 )
  {
	reset_timer(timer_num);
	LPC_TIMER0->TCR = 1;
  }
  else if ( timer_num == 1 )
  {
	reset_timer(timer_num);
	LPC_TIMER1->TCR = 1;
  }
  else if ( timer_num == 2 )
  {
	reset_timer(timer_num);
	LPC_TIMER2->TCR = 1;
  }
  else if ( timer_num == 3 )
  {
	reset_timer(timer_num);
	LPC_TIMER3->TCR = 1;
  }
  return;
}

/******************************************************************************
** Function name:		disable_timer
**
** Descriptions:		Disable timer
**
** parameters:			timer number: 0 or 1 oe 2 or 3
** Returned value:		None
** 
******************************************************************************/
void disable_timer( uint8_t timer_num )
{
  if ( timer_num == 0 )
  {
	LPC_TIMER0->TCR = 0;
  }
  else if ( timer_num == 1 )
  {
	LPC_TIMER1->TCR = 0;
  }
  else if ( timer_num == 2 )
  {
	LPC_TIMER2->TCR = 0;
  }
  else if ( timer_num == 3 )
  {
	LPC_TIMER2->TCR = 0;
  }
  return;
}

/******************************************************************************
** Function name:		reset_timer
**
** Descriptions:		Reset timer
**
** parameters:			timer number: 0 or 1 or 2 or 3
** Returned value:		None
** 
******************************************************************************/
void reset_timer( uint8_t timer_num )
{
  uint32_t regVal;

  if ( timer_num == 0 )
  {
	regVal = LPC_TIMER0->TCR;
	regVal |= 0x02;
	LPC_TIMER0->TCR = regVal;
	LPC_TIMER0->TC = 0;
	counter0 = 0;
  }
  else if ( timer_num == 1 )
  {
	regVal = LPC_TIMER1->TCR;
	regVal |= 0x02;
	LPC_TIMER1->TCR = regVal;
	LPC_TIMER1->TC = 0;
	counter1 = 0;
  }
  else if ( timer_num == 2 )
  {
	regVal = LPC_TIMER2->TCR;
	regVal |= 0x02;
	LPC_TIMER2->TCR = regVal;
	LPC_TIMER2->TC = 0;
	counter2 = 0;
  }
  else if ( timer_num == 3 )
  {
	regVal = LPC_TIMER3->TCR;
	regVal |= 0x02;
	LPC_TIMER3->TCR = regVal;
	LPC_TIMER3->TC = 0;
	counter3 = 0;
  }
  return;
}

/******************************************************************************
** Function name:		init_timer
**
** Descriptions:		Initialize timer, set timer interval, reset timer,
**						install timer interrupt handler
**
** parameters:			timer number and timer interval
** Returned value:		true or false, if the interrupt handler can't be
**						installed, return false.
** 
******************************************************************************/
uint32_t init_timer ( uint8_t timer_num, uint32_t TimerInterval , TimerCallFn callback)
{
  if ( timer_num == 0 )
  {
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_TIMER0);
	Chip_Clock_SetPCLKDiv( 1);

	LPC_SYSCON->PCONP |= (0x01<<1);
	LPC_TIMER0->MR[0] = (SystemCoreClock / TimerInterval);
	LPC_TIMER0->MCR = 3;				/* Interrupt and Reset on MR0 */

	tcb.Timer0_CallBack = callback;
	counter0 = 0;

	NVIC_EnableIRQ(TIMER0_IRQn);
	return (TRUE);
  }
  else if ( timer_num == 1 )
  {
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_TIMER1);
	Chip_Clock_SetPCLKDiv( 1);

	LPC_SYSCON->PCONP |= (0x1<<2);
	LPC_TIMER1->MR[0] = (SystemCoreClock / TimerInterval);
	LPC_TIMER1->MCR = 3;				/* Interrupt and Reset on MR0 */

	tcb.Timer1_CallBack = callback;
	counter1 = 0;

	NVIC_EnableIRQ(TIMER1_IRQn);
	return (TRUE);
  }
  else if ( timer_num == 2 )
  {
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_TIMER1);
	Chip_Clock_SetPCLKDiv( 1);
	LPC_SYSCON->PCONP |= (0x01<<22);
	LPC_TIMER2->MR[0] = (SystemCoreClock / TimerInterval);
	LPC_TIMER2->MCR = 3;				/* Interrupt and Reset on MR0 */

	tcb.Timer2_CallBack = callback;
	counter2 = 0;

	NVIC_EnableIRQ(TIMER2_IRQn);
	return (TRUE);
  }
  else if ( timer_num == 3 )
  {
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_TIMER3);
	Chip_Clock_SetPCLKDiv( 1);
	LPC_SYSCON->PCONP |= (0x01<<23);
	LPC_TIMER3->MR[0] = (SystemCoreClock / TimerInterval);
	LPC_TIMER3->MCR = 3;				/* Interrupt and Reset on MR0 */

	tcb.Timer3_CallBack = callback;
	counter3 = 0;

	NVIC_EnableIRQ(TIMER3_IRQn);
	return (TRUE);
  }
  return (FALSE);
}

/******************************************************************************
**                            End Of File
******************************************************************************/
