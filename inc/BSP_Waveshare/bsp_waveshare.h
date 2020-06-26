/*
 * bsp_waveshare.h
 *
 *  Created on: Dec 12, 2018
 *      Author: dsolano
 */

#ifndef INC_BSP_WAVESHARE_BSP_WAVESHARE_H_
#define INC_BSP_WAVESHARE_BSP_WAVESHARE_H_

#include <chip.h>
#include <lpc_types.h>
#include <stdio.h>
#include "BSP_Waveshare/lpc_norflash.h"
#include "BSP_Waveshare/lpc_nandflash.h"
#include "BSP_Waveshare/lpc_nandflash_k9f1g.h"
#include "BSP_Waveshare/uda1380.h"

/** Define DEBUG_ENABLE to enable IO via the DEBUGSTR, DEBUGOUT, and
    DEBUGIN macros. If not defined, DEBUG* functions will be optimized
    out of the code at build time.
 */
#define DEBUG_ENABLE

/** Define DEBUG_SEMIHOSTING along with DEBUG_ENABLE to enable IO support
    via semihosting. You may need to use a C library that supports
    semihosting with this option.
 */
 //#define DEBUG_SEMIHOSTING

/** wsBoard UART used for debug output and input using the DEBUG* macros. This
    is also the port used for wsBoard_UARTPutChar, wsBoard_UARTGetChar, and
    wsBoard_UARTPutSTR functions.
 */
#define DEBUG_UART LPC_UART0

/**
 * @}
 */
#define I2CDEV_UDA1380_ADDR     0x1A/*!< UDA1380 address */
#define UDA1380_I2C_BUS         I2C0/**< I2C Bus to which UDA1380 is connected */

/* wsBoard name */
#define wsBoard_OPEN_LPC1788


/**
 * LED defines
 */
#define LEDS_LED1           0x01
#define LEDS_LED2           0x02
#define LEDS_LED3           0x04
#define LEDS_LED4           0x08
#define LEDS_NO_LEDS        0x00

/**
 * Button defines
 */
#define BUTTONS_BUTTON1     0x01
#define BUTTONS_BUTTON2     0x02
#define BUTTONS_BUTTON3     0x04
#define NO_BUTTON_PRESSED   0x00

/**
 * Joystick defines
 */
#define JOY_UP              0x01
#define JOY_DOWN            0x02
#define JOY_LEFT            0x04
#define JOY_RIGHT           0x08
#define JOY_PRESS           0x10

#define BUTTONS_BUTTON1_GPIO_PORT_NUM           0
#define BUTTONS_BUTTON1_GPIO_BIT_NUM            10
#define BUTTONS_BUTTON2_GPIO_PORT_NUM           2
#define BUTTONS_BUTTON2_GPIO_BIT_NUM            22
#define BUTTONS_BUTTON3_GPIO_PORT_NUM           4
#define BUTTONS_BUTTON3_GPIO_BIT_NUM            26




#define JOYSTICK_UP_GPIO_PORT_NUM               2
#define JOYSTICK_UP_GPIO_BIT_NUM                26
#define JOYSTICK_DOWN_GPIO_PORT_NUM             2
#define JOYSTICK_DOWN_GPIO_BIT_NUM              23
#define JOYSTICK_LEFT_GPIO_PORT_NUM             2
#define JOYSTICK_LEFT_GPIO_BIT_NUM              25
#define JOYSTICK_RIGHT_GPIO_PORT_NUM            2
#define JOYSTICK_RIGHT_GPIO_BIT_NUM             19
#define JOYSTICK_PRESS_GPIO_PORT_NUM            0
#define JOYSTICK_PRESS_GPIO_BIT_NUM             14

/**
 * @brief Function prototype for a MS delay function. Board layers or example code may
 *        define this function as needed.
 */
typedef void (*p_msDelay_func_t)(uint32_t);

#define USE_RMII	// Waveshare DP83848 has a reduced MII Interface with only 2 bits, thats RMII

void wsBoard_Eth_SetupMuxing(void);

/**
 * @brief	Returns the MAC address assigned to this board
 * @param	mcaddr : Pointer to 6-byte character array to populate with MAC address
 * @return	Nothing
 * @note    Returns the MAC address used by Ethernet
 */
void wsBoard_ENET_GetMacADDR(uint8_t *mcaddr);

/**
 * @brief	Initialize pin muxing for a UART
 * @param	pUART	: Pointer to UART register block for UART pins to init
 * @return	Nothing
 */
void wsBoard_UART_Init(LPC_USART_T *pUART);


/**
 * @brief	Initialize pin muxing for SSP interface
 * @param	pSSP	: Pointer to SSP interface to initialize
 * @return	Nothing
 */
void wsBoard_SSP_Init(LPC_SSP_T *pSSP);

/**
 * @brief	Sets up wsBoard specific I2C interface
 * @param	id	: I2C peripheral ID (I2C0, I2C1 or I2C2)
 * @return	Nothing
 */
void wsBoard_I2C_Init(I2C_ID_T id);

/**
 * @brief	Sets up I2C Fast Plus mode
 * @param	id	: Must always be I2C0
 * @return	Nothing
 * @note	This function must be called before calling
 *          Chip_I2C_SetClockRate() to set clock rates above
 *          normal range 100KHz to 400KHz. Only I2C0 supports
 *          this mode. All I2C slaves of I2C0
 *          are connected to P0.27 (SDA0) P0.28 (SCL0) none of
 *          them will be accessible in fast plus mode because in
 *          fast plus mode P5.2 will be SDA0 and P5.3 will be SCL0,
 *          make sure the Fast Mode Plus supporting slave devices
 *          are connected to these pins.
 */
STATIC INLINE void wsBoard_I2C_EnableFastPlus(I2C_ID_T id)
{
	/* Free P0[27] & P0[28] as SDA0 and SCL0 respectively*/
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 27, IOCON_FUNC0);
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 28, IOCON_FUNC0);

	/* Use P5[2] & P5[3] as SDA0 and SCL0 respectively */
	Chip_IOCON_PinMuxSet(LPC_IOCON, 5, 2, IOCON_FUNC5 | IOCON_HIDRIVE_EN);
	Chip_IOCON_PinMuxSet(LPC_IOCON, 5, 3, IOCON_FUNC5 | IOCON_HIDRIVE_EN);
}

/**
 * @brief	Disable I2C Fast Plus mode and enables default mode
 * @param	id	: Must always be I2C0
 * @return	Nothing
 * @sa		wsBoard_I2C_EnableFastPlus()
 */
STATIC INLINE void wsBoard_I2C_DisableFastPlus(I2C_ID_T id)
{
	/* Free P5[2] & P5[3] as SDA0 and SCL0 respectively */
	Chip_IOCON_PinMuxSet(LPC_IOCON, 5, 2, IOCON_FUNC0);
	Chip_IOCON_PinMuxSet(LPC_IOCON, 5, 3, IOCON_FUNC0);

	/* Use P0[27] & P0[28] as SDA0 and SCL0 respectively*/
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 27, IOCON_FUNC1);
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 28, IOCON_FUNC1);
}

/**
 * @brief	Initialize buttons on the wsBoard
 * @return	Nothing
 */
void wsBoard_Buttons_Init(void);

/**
 * @brief	Get button status
 * @return	status of button
 */
uint8_t wsBoard_Buttons_GetStatus(void);

/**
 * @brief	Initialize Joystick
 * @return	Nothing
 */
void wsBoard_Joystick_Init(void);

/**
 * @brief	Get Joystick status
 * @return	status of Joystick
 */
uint8_t wsBoard_Joystick_GetStatus(void);



/** @defgroup wsBoard_COMMON_API wsBoard: Common wsBoard functions
 * @ingroup wsBoard_Common
 * This file contains common wsBoard definitions that are shared across
 * wsBoards and devices. All of these functions do not need to be
 * implemented for a specific wsBoard, but if they are implemented, they
 * should use this API standard.
 * @{
 */

/**
 * @brief	Setup and initialize hardware prior to call to main()
 * @return	None
 * @note	wsBoard_SystemInit() is called prior to the application and sets up system
 * clocking, memory, and any resources needed prior to the application
 * starting.
 */
void wsBoard_SystemInit(void);

/**
 * @brief	Setup pin multiplexer per wsBoard schematics
 * @return	None
 * @note	wsBoard_SetupMuxing() should be called from SystemInit() prior to application
 * main() is called. So that the PINs are set in proper state.
 */
void wsBoard_SetupMuxing(void);

/**
 * @brief	Setup system clocking
 * @return	None
 * @note	This sets up wsBoard clocking.
 */
void wsBoard_SetupClocking(void);


/**
 * @brief	Set up and initialize all required blocks and functions related to the wsBoard hardware.
 * @return	None
 */
void wsBoard_Init(void);

/**
 * @brief	Initializes wsBoard UART for output, required for printf redirection
 * @return	None
 */
void wsBoard_Debug_Init(void);

/**
 * @brief	Sends a single character on the UART, required for printf redirection
 * @param	ch	: character to send
 * @return	None
 */
void wsBoard_UARTPutChar(char ch);

/**
 * @brief	Get a single character from the UART, required for scanf input
 * @return	EOF if not character was received, or character value
 */
int wsBoard_UARTGetChar(void);

/**
 * @brief	Prints a string to the UART
 * @param	str	: Terminated string to output
 * @return	None
 */
void wsBoard_UARTPutSTR(char *str);

/**
 * @brief	Sets the state of a wsBoard LED to on or off
 * @param	LEDNumber	: LED number to set state for
 * @param	State		: true for on, false for off
 * @return	None
 */
void wsBoard_LED_Set(uint8_t LEDNumber, bool State);

/**
 * @brief	Returns the current state of a wsBoard LED
 * @param	LEDNumber	: LED number to set state for
 * @return	true if the LED is on, otherwise false
 */
bool wsBoard_LED_Test(uint8_t LEDNumber);

/**
 * @brief	Toggles the current state of a wsBoard LED
 * @param	LEDNumber	: LED number to change state for
 * @return	None
 */
void wsBoard_LED_Toggle(uint8_t LEDNumber);

/**
 * @brief	Sets up board specific SDC peripheral
 * @return	Nothing
 */
void wsBoard_SDC_Init(void);


/* The DEBUG* functions are selected based on system configuration.
   Code that uses the DEBUG* functions will have their I/O routed to
   the UART, semihosting, or nowhere. */
#if defined(DEBUG_ENABLE)
#if defined(DEBUG_SEMIHOSTING)
#define DEBUGINIT()
#define DEBUGOUT(...) printf(__VA_ARGS__)
#define DEBUGSTR(str) printf(str)
#define DEBUGIN() (int) EOF

#else
#define DEBUGINIT() wsBoard_Debug_Init()
#define DEBUGOUT(...) printf(__VA_ARGS__)
#define DEBUGSTR(str) wsBoard_UARTPutSTR(str)
#define DEBUGIN() wsBoard_UARTGetChar()
#endif /* defined(DEBUG_SEMIHOSTING) */

#else
#define DEBUGINIT()
#define DEBUGOUT(...)
#define DEBUGSTR(str)
#define DEBUGIN() (int) EOF
#endif /* defined(DEBUG_ENABLE) */

/**
 * @brief	Write on address lines and data lines which are being connected to NOR Flash
 * @param	addr	: value which will be placed on address lines
 * @param	data	: value which will be placed on data lines
 * @return	Nothing
 */
STATIC INLINE void wsBoard_NorFlash_WriteCmd(uint32_t addr, uint16_t data)
{
	*((volatile uint16_t *) (EMC_ADDRESS_CS0 | (addr << 1))) = data;
}

/**
 * @brief	Read command data returned by NOR Flash
 * @param	addr	: value which will be placed on address lines
 * @return	Data returned by NOR Flash
 */
STATIC INLINE uint16_t wsBoard_NorFlash_ReadCmdData(uint32_t addr)
{
	return *((volatile uint16_t *) (EMC_ADDRESS_CS0 | (addr << 1)));
}

/**
 * @brief	Write 16-bit data to NOR Flash
 * @param	addr	: Offset in NOR Flash
 * @param	data	: Data which will be written to NOR Flash
 * @return	Nothing
 */
STATIC INLINE void wsBoard_NorFlash_WriteWord(uint32_t addr, uint16_t data)
{
	*((volatile uint16_t *) (EMC_ADDRESS_CS0 | addr)) = data;
}

/**
 * @brief	Read 16-bit data from NOR Flash
 * @param	addr	: Offset in NOR Flash
 * @return	Nothing
 */
STATIC INLINE uint16_t wsBoard_NorFlash_ReadWord(uint32_t addr)
{
	return *((volatile uint16_t *) (EMC_ADDRESS_CS0 | addr));
}

#if defined(NAND_SUPPORTED_LOCKEDCS)
/**
 * @brief	Lock NAND CS state
 * @param	activeCS	: true to activate CS, false to deactivate
 * @return	Nothing
 */
STATIC INLINE void wsBoard_NANDFLash_CSLatch(bool activeCS)
{
	/* NAND FLASH active GPIO/CS state is low */
	Chip_GPIO_WritePortBit(LPC_GPIO, 4, 31, !activeCS);
}

#endif

#define NANDFLASH_READY_PORT       2
#define NANDFLASH_READY_PIN        21

/**
 * @brief	Read 16-bit data from NOR Flash
 * @return	Nothing
 */
void wsBoard_NANDFLash_Init(void);

/**
 * @brief	Poll NAND Ready/Busy signal
 * @return	true if ready, false if busy
 * @note	Polls the R/nB signal and returns the state
 */
STATIC INLINE bool wsBoard_NANDFLash_GetReady(void)
{
	return (Chip_GPIO_ReadPortBit(LPC_GPIO, NANDFLASH_READY_PORT, NANDFLASH_READY_PIN)) ? true : false;
}

/**
 * @brief	Write a command to NAND Flash
 * @param	cmd	: Command byte
 * @return	Nothing
 */
STATIC INLINE void wsBoard_NANDFLash_WriteCmd(uint8_t cmd)
{
	*((volatile uint8_t *) (EMC_ADDRESS_CS1 | (1 << 20))) = cmd;
}

/**
 * @brief	Write a address byte to NAND Flash
 * @param	addr	: Address byte
 * @return	Nothing
 */
STATIC INLINE void wsBoard_NANDFLash_WriteAddr(uint8_t addr)
{
	*((volatile uint8_t *) (EMC_ADDRESS_CS1 | (1 << 19))) = addr;
}

/**
 * @brief	Write a byte to NAND Flash
 * @param	data	: Data byte
 * @return	Nothing
 */
STATIC INLINE void wsBoard_NANDFLash_WriteByte(uint8_t data)
{
	*((volatile uint8_t *) (EMC_ADDRESS_CS1)) = data;
}

/**
 * @brief	Read data byte from Nand Flash
 * @return	Byte read from NAND FLASH
 */
STATIC INLINE uint8_t wsBoard_NANDFLash_ReadByte(void)
{
	return *((volatile uint8_t *) (EMC_ADDRESS_CS1));
}

/**
 * @brief	Initializes USB device mode pins per board design
 * @param	port	: USB port to be enabled
 * @return	Nothing
 * @note	Only one of the USB port can be enabled at a given time.
 */
void wsBoard_USBD_Init(uint32_t port);

//#include "board_api.h"
#include "BSP_Waveshare/lpc_phy.h"


#endif /* INC_BSP_WAVESHARE_BSP_WAVESHARE_H_ */
