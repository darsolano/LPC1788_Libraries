
#include <hardware_delay.h>

#define PCLK	(SystemCoreClock/2)

// ****************
// _delay - creates a delay of the appropriate number of ticks (happens every 1 us)
void _delay_uS (uint32_t delayTicks) {
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_TIMER1);
	Chip_Clock_SetPCLKDiv( 1);
	TIMER1->CTCR = 0x0;
	TIMER1->PR = SystemCoreClock/1000000;
	TIMER1->TCR = 0x02;	//reset timer match and counter and prescaler
	TIMER1->TCR = 0x01;	// Enable timer0
	while (TIMER1->TC < delayTicks);
	TIMER1->TCR = 0;	// timer0 disable
}

void _delay_ms (uint32_t delayTicks) {
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_TIMER1);
	Chip_Clock_SetPCLKDiv(1);
	TIMER1->CTCR = 0x0;
	TIMER1->PR = SystemCoreClock/1000;
	TIMER1->TCR = 0x02;	//reset timer match and counter and prescaler
	TIMER1->TCR = 0x01;	// Enable timer0
	while (TIMER1->TC < delayTicks);
	TIMER1->TCR = 0;	// timer0 disable
}

/*
 * Setup the timer to start counting @ specified freq.
 * 1000 = milliseconds, 1000000 = microseconds
 */
void initTimer0(uint32_t freq)
{
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_TIMER0);
	Chip_Clock_SetPCLKDiv( 1);

	TIMER0->CTCR = 0x0;
	TIMER0->PR = SystemCoreClock / freq;//PRESCALE; //Increment TC at every 24999+1 clock cycles
	TIMER0->TCR = 0x02; //Reset Timer
}

/*
 * Enable timer and start counting the specified freq.
 */
void startTimer0(void)
{
	TIMER0->TCR = 0x02; //Reset Timer
	TIMER0->TCR = 0x01; //Enable timer
}

/*
 * Stop the timer and returns the value of the counter
 * at stop time, the you use the getTimer0_counter to verify
 * the counting time desired for your app
 */
uint_fast32_t stopTimer0(void)
{
	TIMER0->TCR = 0x00; //Disable timer
	return TIMER0->TC;
}

/*
 * Get the TC timer counter value for your app
 */
uint_fast32_t getTimer0_counter(void)
{
	return TIMER0->TC;
}

//void delayUS(unsigned int microseconds) //Using Timer0
//{
//	TIMER0->TCR = 0x02; //Reset Timer
//	TIMER0->TCR = 0x01; //Enable timer
//	while(TIMER0->TC < microseconds); //wait until timer counter reaches the desired delay
//	TIMER0->TCR = 0x00; //Disable timer
//}
//
//void delayMS(unsigned int milliseconds) //Using Timer0
//{
//	delayUS(milliseconds * 1000);
//}
