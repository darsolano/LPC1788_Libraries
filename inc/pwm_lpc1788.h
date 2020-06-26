/*
 * softPWM.h
 *
 *  Created on: Dec 14, 2018
 *      Author: dsolano
 */

#ifndef INC_PWM_LPC1788_H_
#define INC_PWM_LPC1788_H_

#include <chip.h>
#include <lpc_types.h>
#include <stdint.h>

typedef enum{
	PWM_ID0,
	PWM_ID1
}PWM_ID_t;


typedef struct{
	__IO uint32_t IR;
	__IO uint32_t TCR;
	__IO uint32_t TC;
	__IO uint32_t PR;
	__IO uint32_t PC;
	__IO uint32_t MCR;
	__IO uint32_t MR0;
	__IO uint32_t MR1;
	__IO uint32_t MR2;
	__IO uint32_t MR3;
	__IO uint32_t CCR;
	__IO uint32_t CR0;
	__IO uint32_t CR1;
	__IO uint32_t MR4;
	__IO uint32_t MR5;
	__IO uint32_t MR6;
	__IO uint32_t PCR;
	__IO uint32_t LER;
	__IO uint32_t CTCR;
}LPC_PWM_t;

#define LPC_PWM0	((LPC_PWM_t *) LPC_PWM0_BASE)
#define LPC_PWM1	((LPC_PWM_t *) LPC_PWM1_BASE)

/*
 * PWM Interrupt Register (IR - address 0x4001 4000 (PWM0) and 0x4001 8000 (PWM1)
 */
#define PWM_MR0_INT	(1<<0)
#define PWM_MR1_INT	(1<<1)
#define PWM_MR2_INT	(1<<2)
#define PWM_MR3_INT	(1<<3)

#define PWM_CAP0_INT (1<<4)
#define PWM_CAP1_INT (1<<5)

#define PWM_MR4_INT	(1<<8)
#define PWM_MR5_INT	(1<<9)
#define PWM_MR6_INT	(1<<10)

/*
 * 	PWM Timer Control Register (TCR - address 0x4001 4004 (PWM0) and 0x4001 8004 (PWM1))
 */
#define PWM_TCR_COUNTER_ENABLE		(1<<0)
#define PWM_TCR_COUNTER_RESET		(1<<1)
#define PWM_TCR_PWM_ENABLE			(1<<2)
#define PWM_TCR_PWM0_MASTER_DISABLE	(1<<3)

/*
 * Match Control Register (MCR - address 0x4001 4014 (PWM0) and 0x4001 8014 (PWM1))
 */
#define PWM0_MR0_INT_ENABLE			(1<<0)
#define PWM0_MR0_RESET				(1<<1)
#define PWM0_MR0_STOP				(1<<2)
#define PWM1_MR1_INT_ENABLE			(1<<3)
#define PWM1_MR1_RESET				(1<<4)
#define PWM1_MR1_STOP				(1<<5)
#define PWM0_MR2_INT_ENABLE			(1<<6)
#define PWM0_MR2_RESET				(1<<7)
#define PWM0_MR2_STOP				(1<<8)
#define PWM3_MR3_INT_ENABLE			(1<<9)
#define PWM3_MR3_RESET				(1<<10)
#define PWM3_MR3_STOP				(1<<11)
#define PWM4_MR4_INT_ENABLE			(1<<12)
#define PWM4_MR4_RESET				(1<<13)
#define PWM4_MR4_STOP				(1<<14)
#define PWM5_MR5_INT_ENABLE			(1<<15)
#define PWM5_MR5_RESET				(1<<16)
#define PWM5_MR5_STOP				(1<<17)
#define PWM6_MR6_INT_ENABLE			(1<<18)
#define PWM6_MR6_RESET				(1<<19)
#define PWM6_MR6_STOP				(1<<20)

/*
 * PWM Capture Control Register (CCR - address 0x4001 4028 (PWM0) and 0x4001 8028 (PWM1))
 */
#define CAP_PWMn_CAP0_RISING_EDGE		(1<<0)
#define CAP_PWMn_CAP0_FALLING_EDGE		(1<<1)
#define CAP_PWMn_CAP0_INTERRUPT			(1<<2)
#define CAP_PWMn_CAP1_RISING_EDGE		(1<<3)
#define CAP_PWMn_CAP1_FALLING_EDGE		(1<<4)
#define CAP_PWMn_CAP1_INTERRUPT			(1<<5)

/*
 * The PWM Control registers are used to enable and select the type of each PWM channel.
 * PWM Control Registers (PCR - address 0x4001 404C (PWM0) and 0x4001 804C (PWM1))
 */
#define PWM_SEL2_OUTPUT_SINGLE_DOUBLE_EDGE_MODE_CTRL	(1<<2)
#define PWM_SEL3_OUTPUT_SINGLE_DOUBLE_EDGE_MODE_CTRL	(1<<3)
#define PWM_SEL4_OUTPUT_SINGLE_DOUBLE_EDGE_MODE_CTRL	(1<<4)
#define PWM_SEL5_OUTPUT_SINGLE_DOUBLE_EDGE_MODE_CTRL	(1<<5)
#define PWM_SEL6_OUTPUT_SINGLE_DOUBLE_EDGE_MODE_CTRL	(1<<6)
//*******************
#define PWM_ENA1_OUTPUT_ENABLE_CTRL						(1<<9)
#define PWM_ENA2_OUTPUT_ENABLE_CTRL						(1<<10)
#define PWM_ENA3_OUTPUT_ENABLE_CTRL						(1<<11)
#define PWM_ENA4_OUTPUT_ENABLE_CTRL						(1<<12)
#define PWM_ENA5_OUTPUT_ENABLE_CTRL						(1<<13)
#define PWM_ENA6_OUTPUT_ENABLE_CTRL						(1<<14)
#define CHANNEL_1										9
#define CHANNEL_2										10
#define CHANNEL_3										11
#define CHANNEL_4										12
#define CHANNEL_5										13
#define CHANNEL_6										14
#define PWM_CHANNEL_ENABLE(ch)							(1<<ch)
/*
 * The PWM Latch Enable registers are used to control the update of the PWM Match
registers when they are used for PWM generation. When software writes to the location of
a PWM Match register while the Timer is in PWM mode, the value is actually held in a
shadow register and not used immediately.
When a PWM Match 0 event occurs (normally also resetting the timer in PWM mode), the
contents of shadow registers will be transferred to the actual Match registers if the
corresponding bit in the Latch Enable register has been set. At that point, the new values
will take effect and determine the course of the next PWM cycle. Once the transfer of new
values has taken place, all bits of the LER are automatically cleared. Until the
corresponding bit in the PWMLER is set and a PWM Match 0 event occurs, any value
written to the PWM Match registers has no effect on PWM operation.
For example, if PWM is configured for double edge operation and is currently running, a
typical sequence of events for changing the timing would be:
• Write a new value to the PWM Match1 register.
• Write a new value to the PWM Match2 register.
• Write to the PWMLER, setting bits 1 and 2 at the same time.
• The altered values will become effective at the next reset of the timer (when a PWM
Match 0 event occurs).
The order of writing the two PWM Match registers is not important, since neither value will
be used until after the write to PWMLER. This insures that both values go into effect at the
same time, if that is required. A single value may be altered in the same way if needed.
 */
#define MR0_MATCH_LATCH_ENABLE			(0)
#define MR1_MATCH_LATCH_ENABLE			(1)
#define MR2_MATCH_LATCH_ENABLE			(2)
#define MR3_MATCH_LATCH_ENABLE			(3)
#define MR4_MATCH_LATCH_ENABLE			(4)
#define MR5_MATCH_LATCH_ENABLE			(5)
#define MR6_MATCH_LATCH_ENABLE			(6)
#define PWM_LER_ENABLE(n)				(1<<n)

/*
 * PWM Count Control Register
The Count Control Register (CTCR) is used to select between Timer and Counter mode,
and in Counter mode to select the pin and edges for counting. The function of each of the
bits is shown in Table 571.
Remark: PWM_CAP input signal frequency must not exceed PCLK/4. When the PWM
clock is supplied via the PWM_CAP pin, at no time can a high or low level of the signal on
this pin can last less than 1/(2xPCLK).
 */
#define PWM_CCR_MODE(mode)					(mode<<0)
#define PWM_CCR_MODE_TIMER_MODE				0
#define PWM_CCR_MODE_RISING_EDGE_CTR_MODE	1
#define PWM_CCR_MODE_FALLING_EDGE_CTR_MODE	2
#define PWM_CCR_MODE_DUAL_EDGE_CTR_MODE		3

#define PWM_CCR_CIS(select)					(select<<2)
#define PWM_CCR_CIS_PWM0_CAP0				0
#define PWM_CCR_CIS_PWM1_CAP0				0
#define PWM_CCR_CIS_PWM1_CAP1				1


#endif /* INC_PWM_LPC1788_H_ */
