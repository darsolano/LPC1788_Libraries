/*
 * bsp_waveshare.c
 *
 *  Created on: Dec 12, 2018
 *      Author: dsolano
 */

#include <BSP_Waveshare/bsp_waveshare.h>
#include <gfx/open1788_bsp.h>
#include "gfx/sdram_HY57V281620_X2.h"
#include "string.h"

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/
#define UART_ACCEPTED_BAUDRATE_ERROR	(3)

/* System oscillator rate and RTC oscillator rate */
const uint32_t OscRateIn = 12000000;
const uint32_t RTCOscRateIn = 32768;

/* Pin muxing configuration */
STATIC const PINMUX_GRP_T pinmuxing[] = {
	/* CAN RD1 and TD1 */
	{0x0, 0,  (IOCON_FUNC1 | IOCON_MODE_INACT)},
	{0x0, 1,  (IOCON_FUNC1 | IOCON_MODE_INACT)},

	/* UART 0 debug port (via USB bridge) */
	{0x0, 2,  (IOCON_FUNC1 | IOCON_MODE_INACT)},
	{0x0, 3,  (IOCON_FUNC1 | IOCON_MODE_INACT)},

	/* I2S */
	{0x0, 4,  (IOCON_FUNC1 | IOCON_MODE_INACT)},	/* I2S RX clock */
	{0x0, 5,  (IOCON_FUNC1 | IOCON_MODE_INACT)},	/* I2S RX WS */
	{0x0, 6,  (IOCON_FUNC1 | IOCON_MODE_INACT)},	/* I2S RX SDA */
	{0x0, 7,  (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGMODE_EN)},	/* I2S TX clock */
	{0x0, 8,  (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGMODE_EN)},	/* I2S TX WS */
	{0x0, 9,  (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGMODE_EN)},	/* I2S TX SDA */

	{0x0, 13, (IOCON_FUNC1 | IOCON_MODE_INACT)},	/* USB LED */
	{0x0, 14, (IOCON_FUNC3 | IOCON_MODE_INACT)},	/* USB Softconnect */
	/* SSP 0 */
	{0x0, 15, (IOCON_FUNC1 | IOCON_MODE_INACT)},	/* SSP CLK */
	//{0x0, 16, (IOCON_FUNC1 | IOCON_MODE_INACT)},
	{0x0, 17, (IOCON_FUNC1 | IOCON_MODE_INACT)},
	{0x0, 18, (IOCON_FUNC1 | IOCON_MODE_INACT)},
	/* ADC */
	{0x0, 25, (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_ADMODE_EN)},
	/* DAC */
	{0x0, 26, (IOCON_FUNC2 | IOCON_DAC_EN | IOCON_HYS_EN | IOCON_MODE_PULLUP)},
	/* USB */
	{0x0, 29, (IOCON_FUNC1 | IOCON_MODE_INACT)},
	{0x0, 30, (IOCON_FUNC1 | IOCON_MODE_INACT)},
	{0x0, 31, (IOCON_FUNC1 | IOCON_MODE_INACT)},

	/* ENET */
	{0x1, 0, (IOCON_FUNC1 | IOCON_MODE_INACT)},	//RMII_TXD0
	{0x1, 1, (IOCON_FUNC1 | IOCON_MODE_INACT)}, //RMII_TXD1
	{0x1, 4, (IOCON_FUNC1 | IOCON_MODE_INACT)}, //RMII_TX_EN
	{0x1, 8, (IOCON_FUNC1 | IOCON_MODE_INACT)}, //RMII_CRS_DV
	{0x1, 9, (IOCON_FUNC1 | IOCON_MODE_INACT)}, //RMII_RXD0
	{0x1, 10, (IOCON_FUNC1 | IOCON_MODE_INACT)},//RMII_RXD1
	//{0x1, 14, (IOCON_FUNC1 | IOCON_MODE_INACT)},
	{0x1, 15, (IOCON_FUNC1 | IOCON_MODE_INACT)}, //RMII_REF_CLK
	{0x1, 16, (IOCON_FUNC1 | IOCON_MODE_INACT)},
	{0x1, 17, (IOCON_FUNC1 | IOCON_MODE_INACT)},//RMII_MDIO
	{0x1, 27, (IOCON_FUNC4 | IOCON_MODE_INACT | IOCON_DIGMODE_EN)}, /* CLKOUT */

	/* JOYSTICK */
	{2, 26, (IOCON_FUNC0 | IOCON_MODE_INACT)}, 	/* JOYSTICK_UP */
	{2, 23, (IOCON_FUNC0 | IOCON_MODE_INACT)}, 	/* JOYSTICK_DOWN */
	{2, 25, (IOCON_FUNC0 | IOCON_MODE_INACT)}, 	/* JOYSTICK_LEFT */
	{2, 27, (IOCON_FUNC0 | IOCON_MODE_INACT)}, 	/* JOYSTICK_RIGHT */
	{2, 22, (IOCON_FUNC0 | IOCON_MODE_INACT)}, 	/* JOYSTICK_PRESS */

	/*   For the EA LPC1788 VBUS is not connected.  Leave it as GPIO. */
	/* {0x1, 30, (IOCON_FUNC2 | IOCON_MODE_INACT)}, */ /* USB_VBUS */

	/* FIXME NOT COMPLETE */

	/* LEDs */
	{0x2, 26, (IOCON_FUNC0 | IOCON_MODE_INACT)},
	{0x2, 27, (IOCON_FUNC0 | IOCON_MODE_INACT)},
};

STATIC const PINMUX_GRP_T eth_pinmuxing[] = {
/* ENET */
{0x1, 0, (IOCON_FUNC1 | IOCON_MODE_INACT)},	//RMII_TXD0
{0x1, 1, (IOCON_FUNC1 | IOCON_MODE_INACT)}, //RMII_TXD1
{0x1, 4, (IOCON_FUNC1 | IOCON_MODE_INACT)}, //RMII_TX_EN
{0x1, 8, (IOCON_FUNC1 | IOCON_MODE_INACT)}, //RMII_CRS_DV
{0x1, 9, (IOCON_FUNC1 | IOCON_MODE_INACT)}, //RMII_RXD0
{0x1, 10, (IOCON_FUNC1 | IOCON_MODE_INACT)},//RMII_RXD1
{0x1, 15, (IOCON_FUNC1 | IOCON_MODE_INACT)}, //RMII_REF_CLK
{0x1, 17, (IOCON_FUNC1 | IOCON_MODE_INACT)}, //RMII_MDIO
{0x2, 8, (IOCON_FUNC4 | IOCON_MODE_INACT)}, //RMII_MDC

};

/* Sets up system pin muxing */
void wsBoard_Eth_SetupMuxing(void)
{
	//int i, j;

	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_EMC);
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_ENET);

	Chip_SYSCTL_PeriphReset(SYSCTL_RESET_IOCON);

	/* Setup data, address, and EMC control pins with high slew rate */
//	for (i = 3; i <= 4; i++) {
//		for (j = 0; j <= 31; j++) {
//			Chip_IOCON_PinMuxSet(LPC_IOCON, (uint8_t) i, (uint8_t) j, (IOCON_FUNC1 | IOCON_FASTSLEW_EN));
//		}
//	}
//	for (i = 16; i <= 31; i++) {
//		Chip_IOCON_PinMuxSet(LPC_IOCON, 2, (uint8_t) i, (IOCON_FUNC1 | IOCON_FASTSLEW_EN));
//	}

	/* Setup system level pin muxing */
	Chip_IOCON_SetPinMuxing(LPC_IOCON, eth_pinmuxing, sizeof(eth_pinmuxing) / sizeof(PINMUX_GRP_T));
}

/*
#define MYMAC_1         0x1E
#define MYMAC_2         0x30
#define MYMAC_3         0x6c
#define MYMAC_4         0xa2
#define MYMAC_5         0x45
#define MYMAC_6         0x5e
*/
/* Returns the MAC address assigned to this board */
void wsBoard_ENET_GetMacADDR(uint8_t *mcaddr)
{
	const uint8_t boardmac[] = {0x1E, 0x30, 0x6c, 0xa2, 0x45, 0x5e};

	memcpy(mcaddr, boardmac, 6);
}

static void wsBoard_UARTx_SetBaudRate(LPC_USART_T *UARTx, int BaudRate){
	Status errorStatus = ERROR;

		uint32_t uClk;
		uint32_t d, m, bestd, bestm, tmp;
		uint64_t best_divisor, divisor;
		uint32_t current_error, best_error;
		uint32_t recalcbaud;

		/* get UART block clock */
		uClk = Chip_Clock_GetPeripheralClockRate();

		/* In the Uart IP block, baud rate is calculated using FDR and DLL-DLM registers
		* The formula is :
		* BaudRate= uClk * (mulFracDiv/(mulFracDiv+dividerAddFracDiv) / (16 * (DLL)
		* It involves floating point calculations. That's the reason the formulae are adjusted with
		* Multiply and divide method.*/

		/* The value of mulFracDiv and dividerAddFracDiv should comply to the following expressions:
		* 0 < mulFracDiv <= 15, 0 <= dividerAddFracDiv <= 15 */
		best_error = 0xFFFFFFFF; /* Worst case */
		bestd = 0;
		bestm = 0;
		best_divisor = 0;

		for (m = 1 ; m <= 15 ;m++)
		{
			for (d = 0 ; d < m ; d++)
			{
				divisor = ((uint64_t)uClk << 28)*m / (BaudRate*(m+d));
				current_error = divisor & 0xFFFFFFFF;

				tmp = divisor>>32;

				/* Adjust error */
				if(current_error > ((uint32_t)1<<31))
				{
					current_error = -current_error;
					tmp++;
				}

				/* Out of range */
				if(tmp < 1 || tmp > 65536)
					continue;

				if( current_error < best_error)
				{
					best_error = current_error;
					best_divisor = tmp;
					bestd = d;
					bestm = m;

					if(best_error == 0)
						break;
				}
			} /* end of inner for loop */

			if (best_error == 0)
				break;
		} /* end of outer for loop  */

		/* can not find best match */
		if(best_divisor == 0)
			return ;//ERROR;

		recalcbaud = (uClk >> 4) * bestm / (best_divisor * (bestm + bestd));

		/* reuse best_error to evaluate baud error*/
		if(BaudRate > recalcbaud)
			best_error = BaudRate - recalcbaud;
		else
			best_error = recalcbaud -BaudRate;

		best_error = best_error * 100 / BaudRate;

		if (best_error < UART_ACCEPTED_BAUDRATE_ERROR)
		{
			if ((UARTx) == LPC_UART1)
			{
				(UARTx)->LCR |= UART_LCR_DLAB_EN;

				(UARTx)->DLM = UART_LOAD_DLM(best_divisor);

				(UARTx)->DLL = UART_LOAD_DLL(best_divisor);

				/* Then reset DLAB bit */
				(UARTx)->LCR &= (~UART_LCR_DLAB_EN) & UART_LCR_BITMASK;

				(UARTx)->FDR = (UART_FDR_MULVAL(bestm)
														| UART_FDR_DIVADDVAL(bestd)) & UART_FDR_BITMASK;
			}
			else
			{
				UARTx->LCR |= UART_LCR_DLAB_EN;

				UARTx->DLM = UART_LOAD_DLM(best_divisor);

				UARTx->DLL = UART_LOAD_DLL(best_divisor);

				/* Then reset DLAB bit */
				UARTx->LCR &= (~UART_LCR_DLAB_EN) & UART_LCR_BITMASK;

				UARTx->FDR = (UART_FDR_MULVAL(bestm) \
								| UART_FDR_DIVADDVAL(bestd)) & UART_FDR_BITMASK;
			}
			errorStatus = SUCCESS;
		}

		return ;//errorStatus;
}
/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Initialize UART pins */
void wsBoard_UART_Init(LPC_USART_T *pUART)
{
	if (pUART == LPC_UART0) {
		/*
		 * Initialize UART0 pin connect
		 * P0.2: TXD
		 * P0.3: RXD
		 */
		Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_UART0);
		Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 2, (IOCON_FUNC1 | IOCON_MODE_INACT));
		Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 3, (IOCON_FUNC1 | IOCON_MODE_INACT));
	}
	else if (pUART == LPC_UART2) {
		Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_UART2);
		/* Initialize UART2 pin connect */
		Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 10, (IOCON_FUNC1 | IOCON_MODE_INACT));
		Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 11, (IOCON_FUNC1 | IOCON_MODE_INACT));
	}
}

/* Initialize debug output via UART for board */
void wsBoard_Debug_Init(void)
{
#if defined(DEBUG_UART)
	wsBoard_UART_Init(DEBUG_UART);

	Chip_UART_Init(DEBUG_UART);
	Chip_UART_SetBaud(DEBUG_UART, 115200);
	//wsBoard_UARTx_SetBaudRate(DEBUG_UART, 115200);
	Chip_UART_ConfigData(DEBUG_UART, UART_LCR_WLEN8 | UART_LCR_SBS_1BIT | UART_LCR_PARITY_DIS);

	/* Enable UART Transmit */
	Chip_UART_TXEnable(DEBUG_UART);
#endif
}

/* Sends a character on the UART */
void wsBoard_UARTPutChar(char ch)
{
#if defined(DEBUG_UART)
	while ((Chip_UART_ReadLineStatus(DEBUG_UART) & UART_LSR_THRE) == 0) {}
	Chip_UART_SendByte(DEBUG_UART, (uint8_t) ch);
#endif
}

/* Gets a character from the UART, returns EOF if no character is ready */
int wsBoard_UARTGetChar(void)
{
#if defined(DEBUG_UART)
	if (Chip_UART_ReadLineStatus(DEBUG_UART) & UART_LSR_RDR) {
		return (int) Chip_UART_ReadByte(DEBUG_UART);
	}
#endif
	return EOF;
}

/* Outputs a string on the debug UART */
void wsBoard_UARTPutSTR(char *str)
{
#if defined(DEBUG_UART)
	while (*str != '\0') {
		wsBoard_UARTPutChar(*str++);
	}
#endif
}

#define MAXLEDS 3
static const uint8_t ledports[MAXLEDS] = {1, /*0,*/ 1, 4};		// fixed by dsolano
static const uint8_t ledpins[MAXLEDS] = {14, /*16,*/ 13, 27};	// fixed by dsolano

/* Initializes board LED(s) */
static void wsBoard_LED_Init(void)
{
	int i;

	/* Setup port direction and initial output state */
	for (i = 0; i < MAXLEDS; i++) {
		Chip_GPIO_WriteDirBit(LPC_GPIO, ledports[i], ledpins[i], true);
		Chip_GPIO_WritePortBit(LPC_GPIO, ledports[i], ledpins[i], false);
	}
}

/* Sets the state of a board LED to on or off */
void wsBoard_LED_Set(uint8_t LEDNumber, bool On)
{
	if (LEDNumber < MAXLEDS) {
		/* Set state, low is on, high is off */
		if (On) {	/* Set Port */
			LPC_GPIO[ledports[LEDNumber]].SET |= 1UL << ledpins[LEDNumber];
			}
			else {	/* Clear Port */
				LPC_GPIO[ledports[LEDNumber]].CLR |= 1UL << ledpins[LEDNumber];
			}
	}
}

/* Returns the current state of a board LED */
bool wsBoard_LED_Test(uint8_t LEDNumber)
{
	bool state = false;

	if (LEDNumber < MAXLEDS) {
		state = Chip_GPIO_GetPinState(LPC_GPIO, ledports[LEDNumber], ledpins[LEDNumber]);
	}

	/* These LEDs are reverse logic. */
	return !state;
}

/* Toggles the current state of a board LED */
void wsBoard_LED_Toggle(uint8_t LEDNumber)
{
	if (LEDNumber < MAXLEDS) {
		Chip_GPIO_SetPinToggle(LPC_GPIO, ledports[LEDNumber], ledpins[LEDNumber]);
	}
}


/*
 * BasBoard push buttons definition
 */
#define NUM_PUSH_BUTTONS	3
static const uint8_t port_Push_Buttons[NUM_PUSH_BUTTONS] = {
		BUTTONS_BUTTON1_GPIO_PORT_NUM,
		BUTTONS_BUTTON2_GPIO_PORT_NUM,
		BUTTONS_BUTTON3_GPIO_PORT_NUM
};
static const uint8_t pin_Push_Buttons[NUM_PUSH_BUTTONS] = {
		BUTTONS_BUTTON1_GPIO_BIT_NUM,
		BUTTONS_BUTTON2_GPIO_BIT_NUM,
		BUTTONS_BUTTON3_GPIO_BIT_NUM
};
static const uint8_t statePushButton[NUM_PUSH_BUTTONS] = {
		BUTTONS_BUTTON1,
		BUTTONS_BUTTON2,
		BUTTONS_BUTTON3
};


void wsBoard_Buttons_Init(void)
{
	int ix;

	/* IOCON states already selected in SystemInit(), GPIO setup only. Pullups
	   are external, so IOCON with no states */
	for (ix = 0; ix < NUM_PUSH_BUTTONS; ix++) {
		Chip_GPIO_SetPinDIRInput(LPC_GPIO, port_Push_Buttons[ix], pin_Push_Buttons[ix]);
	}
}

uint8_t wsBoard_Buttons_GetStatus(void)
{
	uint8_t ix, ret = 0;

	for (ix = 0; ix < NUM_PUSH_BUTTONS; ix++) {
		while ((Chip_GPIO_GetPinState(LPC_GPIO, port_Push_Buttons[ix], pin_Push_Buttons[ix])) == false) {
			ret |= statePushButton[ix];
		}
	}

	return ret;
}


/* Baseboard joystick buttons */
#define NUM_BUTTONS 5
static const uint8_t portButton[NUM_BUTTONS] = {
	JOYSTICK_UP_GPIO_PORT_NUM,
	JOYSTICK_DOWN_GPIO_PORT_NUM,
	JOYSTICK_LEFT_GPIO_PORT_NUM,
	JOYSTICK_RIGHT_GPIO_PORT_NUM,
	JOYSTICK_PRESS_GPIO_PORT_NUM
};
static const uint8_t pinButton[NUM_BUTTONS] = {
	JOYSTICK_UP_GPIO_BIT_NUM,
	JOYSTICK_DOWN_GPIO_BIT_NUM,
	JOYSTICK_LEFT_GPIO_BIT_NUM,
	JOYSTICK_RIGHT_GPIO_BIT_NUM,
	JOYSTICK_PRESS_GPIO_BIT_NUM
};
static const uint8_t stateButton[NUM_BUTTONS] = {
	JOY_UP,
	JOY_DOWN,
	JOY_LEFT,
	JOY_RIGHT,
	JOY_PRESS
};

/* Initialize Joystick */
void wsBoard_Joystick_Init(void)
{
	int ix;

	/* IOCON states already selected in SystemInit(), GPIO setup only. Pullups
	   are external, so IOCON with no states */
	for (ix = 0; ix < NUM_BUTTONS; ix++) {
		Chip_GPIO_SetPinDIRInput(LPC_GPIO, portButton[ix], pinButton[ix]);
	}
}

/* Get Joystick status */
uint8_t wsBoard_Joystick_GetStatus(void)
{
	uint8_t ix, ret = 0;

	for (ix = 0; ix < NUM_BUTTONS; ix++) {
		while ((Chip_GPIO_GetPinState(LPC_GPIO, portButton[ix], pinButton[ix])) == false) {
			ret |= stateButton[ix];
		}
	}

	return ret;
}

/* Sets up board specific I2C interface */
void wsBoard_I2C_Init(I2C_ID_T id)
{
	switch (id) {
		case I2C0:
			Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 27, IOCON_FUNC1);
			Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 28, IOCON_FUNC1);
			break;

		case I2C1:
			Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 19, (IOCON_FUNC3 | IOCON_MODE_PULLUP | IOCON_OPENDRAIN_EN));
			Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 20, (IOCON_FUNC3 | IOCON_MODE_PULLUP | IOCON_OPENDRAIN_EN));
			break;

		case I2C2:
			Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 15, (IOCON_FUNC3 | IOCON_MODE_PULLUP | IOCON_OPENDRAIN_EN));
			Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 11, (IOCON_FUNC2 | IOCON_MODE_PULLUP | IOCON_OPENDRAIN_EN));
			break;

		default:
			return;
	}
}

/* Initialize pin muxing for SSP interface */
void wsBoard_SSP_Init(LPC_SSP_T *pSSP)
{
	if (pSSP == LPC_SSP1) {
		/* Set up clock and muxing for SSP1 interface */
		/*
		 * Initialize SSP1 pins connect
		 * P1.19: SCK
		 * P0.14: SSEL
		 * P0.12: MISO
		 * P0.13: MOSI
		 */
		Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 19, (IOCON_FUNC5 | IOCON_MODE_INACT | IOCON_DIGMODE_EN));
		Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 14, (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_DIGMODE_EN));
		Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 12, (IOCON_FUNC2 | IOCON_MODE_INACT | IOCON_DIGMODE_EN));
		Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 13, (IOCON_FUNC2 | IOCON_MODE_INACT | IOCON_DIGMODE_EN));
	}
//	else if (pSSP == LPC_SSP2) {	commented by dsolano
//		/*
//		 * Initialize SSP2 pins connect
//		 * P5.2: SCK
//		 * P5.3: SSEL
//		 * P5.1: MISO
//		 * P5.0: MOSI
//		 */
//		Chip_IOCON_PinMuxSet(LPC_IOCON, 5, 2, (IOCON_FUNC2 | IOCON_MODE_INACT));
//		Chip_IOCON_PinMuxSet(LPC_IOCON, 5, 3, (IOCON_FUNC2 | IOCON_MODE_INACT));
//		Chip_IOCON_PinMuxSet(LPC_IOCON, 5, 1, (IOCON_FUNC2 | IOCON_MODE_INACT));
//		Chip_IOCON_PinMuxSet(LPC_IOCON, 5, 0, (IOCON_FUNC2 | IOCON_MODE_INACT));

//	}
	else if (pSSP == LPC_SSP0){
		/* Set up clock and muxing for SSP0 interface */
		/*
		 * Initialize SSP0 pins connect
		 * P1.20: SCK
		 * P1.21: SSEL
		 * P1.23: MISO
		 * P1.24: MOSI
		 */
		Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 20, (IOCON_FUNC5 | IOCON_MODE_INACT));
		Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 21, (IOCON_FUNC0 | IOCON_MODE_INACT));
		Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 23, (IOCON_FUNC5 | IOCON_MODE_INACT));
		Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 24, (IOCON_FUNC5 | IOCON_MODE_INACT));
	}
}

/* Set up and initialize all required blocks and functions related to the
   board hardware */
void wsBoard_Init(void)
{
	Chip_Clock_SetPCLKDiv(1);

	/* Initializes GPIO */
	Chip_GPIO_Init(LPC_GPIO);
	Chip_IOCON_Init(LPC_IOCON);

	/* Sets up DEBUG UART */
	DEBUGINIT();


	/* Initialize LEDs, Joystick, buttons and LCD Screen */
	wsBoard_LED_Init();
	wsBoard_Joystick_Init();
	wsBoard_Buttons_Init();
	SDRAMInit();
	//open1788_LCDinit();
	//open1788_LCD_Clear(Black);
}


/* Setup board for SDC interface */
void wsBoard_SDC_Init(void)
{
	Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 6, 	(IOCON_FUNC2 | IOCON_MODE_INACT));	// SDIO D0
	Chip_IOCON_PinMuxSet(LPC_IOCON, 2, 11, 	(IOCON_FUNC2 | IOCON_MODE_INACT));	// SDIO D1
	Chip_IOCON_PinMuxSet(LPC_IOCON, 2, 12, 	(IOCON_FUNC2 | IOCON_MODE_INACT));	// SDIO D2
	Chip_IOCON_PinMuxSet(LPC_IOCON, 2, 13, 	(IOCON_FUNC2 | IOCON_MODE_INACT));	// SDIO D3
	Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 2, 	(IOCON_FUNC2 | IOCON_MODE_INACT));	// SDIO CLK
	Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 3, 	(IOCON_FUNC2 | IOCON_MODE_INACT));	// SDIO CMD
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 13, 	(IOCON_FUNC0 | IOCON_MODE_INACT)); 	// Card Detect
	Chip_SDC_Init(LPC_SDC);

}


/* Setup system clocking */
void wsBoard_SetupClocking(void)
{
	/* Enable PBOOST for CPU clock over 100MHz */
	Chip_SYSCTL_EnableBoost();

	Chip_SetupXtalClocking();

	/* SPIFI clocking will be derived from Main PLL with a divider of 2 (60MHz) */
	Chip_Clock_SetSPIFIClockDiv(2);
	Chip_Clock_SetSPIFIClockSource(SYSCTL_SPIFICLKSRC_MAINPLL);

}

/* Sets up system pin muxing */
void wsBoard_SetupMuxing(void)
{
	Chip_SYSCTL_PeriphReset(SYSCTL_RESET_IOCON);


	/* Setup system level pin muxing */
	//Chip_IOCON_SetPinMuxing(LPC_IOCON, pinmuxing, sizeof(pinmuxing) / sizeof(PINMUX_GRP_T));
	wsBoard_Eth_SetupMuxing();
}


/* Set up and initialize hardware prior to call to main */
void wsBoard_SystemInit(void)
{
	wsBoard_SetupMuxing();
	wsBoard_SetupClocking();
}
