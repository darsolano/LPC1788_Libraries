/*
 * File:   keypad_4x4.h
 * Author: Daruin O. Solano
 * PIC32MC360F512L ported to LPC17xx NXP
 * Created on June 28, 2014, 11:43 AM
 */
#include <chip.h>
#include <hardware_delay.h>
#include <mcp23008.h>
#include <keypad_4x4.h>
#include <ring_buffer.h>

#define KP_RINGBUFFER_SIZE		12
I2C_ID_T i2cID;
uint8_t i2caddr;
//-----------------------------------------------------------------------------------------
//					Local Functions and Variables
//-----------------------------------------------------------------------------------------
PRIVATE uint8_t kp_KeyScan ( void );
PRIVATE void kp_InvertConfig(void);

//-----------------------------------------------------------------------------------------
//					Local Variables
//-----------------------------------------------------------------------------------------
char kp_Command2Process[12];	// Ring buffer should be copied to this buffer in order to process the current entered command
RINGBUFF_T kp_rb;		// Ring Buffer definition for UART3
volatile char* KPrbBuffer[KP_RINGBUFFER_SIZE];
Bool kpCmdState;

//-----------------------------------------------------------------------------------------
//					END of Local Variables
//-----------------------------------------------------------------------------------------


PRIVATE uint8_t kp_KeyScan ( void )
{
	uint8_t Col = 0;	// Lower 4 Bits
	uint8_t Row = 0;	// Upper 4 Bits
	uint8_t debounce=0;

	mcp23008_WriteGPIO(0xF0, i2caddr);
	for(debounce=0;debounce<5;debounce++)
		Row = mcp23008_ReadINTCAP(i2caddr);	// Reads row status

	kp_InvertConfig();									// Invert the flow of data
	_delay_ms(5);

	for(debounce=0;debounce<5;debounce++)				// Reads cols status
		Col = mcp23008_ReadGPIO(i2caddr);

	// return to normal state
	uint8_t comval = 0xF0;	// Common value for most register that act for IO and Interrupts

	// SET GPIO DIRECTION
	MCP23008_IODIR_REG_sType gpiodir;
	gpiodir.IODir_reg = comval;
	mcp23008_SetGPIODir(&gpiodir,i2caddr);

	// WRITE GPIO INITIAL VALUES
	mcp23008_WriteGPIO(comval, i2caddr);

	// SET PULLUPS FOR OUTPUT PIN ON GPIO 4-7
	mcp23008_SetGPIOPullUps(0x0F, i2caddr);

	// SET INPUT POLARITY AS NORMAL INPUTS
	mcp23008_SetIPOL(0,i2caddr);

	// SET INTERRUPT VALUES FOR BITS 0-3 AS INPUTS
	mcp23008_SetGPIOInt(comval, i2caddr);

	_delay_ms(5);

	mcp23008_ReadINTCAP(i2caddr);				// to clear interrupt
	return (Row | Col);
}



/*
 * Invert the original state of Keypad configuration to get col read
 */
PRIVATE void kp_InvertConfig(void){
	MCP23008_IODIR_REG_sType iodir;
	iodir.IODir_reg = 0x0F;
	mcp23008_SetGPIODir(&iodir,i2caddr);

	mcp23008_WriteGPIO(0x0F, i2caddr);

	mcp23008_SetGPIOInt(0x0F,i2caddr);
}


//-----------------------------------------------------------------------------------------
//					End of Local Functions
//-----------------------------------------------------------------------------------------

int kp_Init (I2C_ID_T i2cx, uint8_t i2cadrx )
{
	uint8_t comval = 0xF0;	// Common value for most register that act for IO and Interrupts

	i2cID   = i2cx;
	i2caddr = i2cadrx;

	Chip_I2C_Init(i2cx);
	Chip_I2C_SetClockRate(i2cx, 400000);
	Chip_I2C_SetMasterEventHandler(i2cx, Chip_I2C_EventHandlerPolling);

	// to check if device is present on i2c bus
	if (!Chip_I2C_MasterSend(i2cx, i2caddr, &comval, 1))
		return 0;

	MCP23008_IOCON_REG_sType iocon;
	iocon.DISSLW 	= 0;	// Slew Rate Control Enable = 0
	iocon.HAEN 		= 1; 	// Hardware address enable
	iocon.INTPOL 	= 0; 	// Int polarity active low
	iocon.ODR 		= 0; 	// ODR for INT pin not open drain, INTPOL determines the action pin
	iocon.SEQOP 	= 1; 	// Sequential op disable
	mcp23008_ConfigIOCON(&iocon,i2caddr);

	// SET GPIO DIRECTION
	MCP23008_IODIR_REG_sType gpiodir;
	gpiodir.IODir_reg = comval;
	mcp23008_SetGPIODir(&gpiodir,i2caddr);

	// WRITE GPIO INITIAL VALUES
	mcp23008_WriteGPIO(comval, i2caddr);

	// SET PULLUPS FOR OUTPUT PIN ON GPIO 4-7
	mcp23008_SetGPIOPullUps(0x0F, i2caddr);

	// SET INPUT POLARITY AS NORMAL INPUTS
	mcp23008_SetIPOL(0,i2caddr);

	// SET INTERRUPT VALUES FOR BITS 0-3 AS INPUTS
	mcp23008_SetGPIOInt(comval, i2caddr);
	return 1;
}


uint8_t  kp_GetKeyPressed(void){
	uint8_t keypressed = 0;
	if (mcp23008_ReadINTF(i2caddr)){	// Verify that any bit from 0-3 is set for interrupt
		keypressed = kp_KeyScan();				// Performs the Keyscan for Row and Columns
		_delay_ms(200);
	}
	else return keypressed;

	switch (keypressed){
	case 0x77:
		return '1';
		break;
	case 0x7B:
		return '2';
		break;
	case 0x7D:
		return '3';
		break;
	case 0x7E:
		return 'A';
		break;
	case 0xB7:
		return '4';
		break;
	case 0xBB:
		return '5';
		break;
	case 0xBD:
		return '6';
		break;
	case 0xBE:
		return 'B';
		break;
	case 0xD7:
		return '7';
		break;
	case 0xDB:
		return '8';
		break;
	case 0xDD:
		return '9';
		break;
	case 0xDE:
		return 'C';
		break;
	case 0xE7:
		return '*';
		break;
	case 0xEB:
		return '0';
		break;
	case 0xED:
		return '#';
		break;
	case 0xEE:
		return 'D';
		break;

	default:
		return 0x00;
		break;
	}
}




