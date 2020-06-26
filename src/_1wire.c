/*********************************************************************
 *
 *                 1-Wire Communication Protocol
 *
 *********************************************************************
 * FileName:        1wire.c
 * Dependencies:
 * Processor:       LPC-1769
 * Complier:        LPCXpresso 7
 * Company:         NXP Semiconductors
 *
 * Software License Agreement
 *
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Sasha     			12/20/07    Original
 ********************************************************************/
 /****** I N C L U D E S **********************************************************/
#include <_1wire.h>
#include <hardware_delay.h>
#include <define_pins.h>


/**PORT D E F I N I T I O N S ****************************************************/

//ONE WIRE PORT PIN DEFINITION
///****************************************************
// This Configuration is required to make any PIC MicroController
// I/O pin as Open drain to drive 1-wire.
///****************************************************
#define  OW_IO_PINS			LPC_IOCON

typedef struct{
	uint8_t port;
	uint8_t pin;
}OW_t;
OW_t ow;

// Input or Output
/**********************************************************************
* Function:        static void OW_PIN_DIRECTION (DIRECTION_eType direction)
* PreCondition:    None
* Input:		   pin direction as INPUT or OUTPUT
* Output:		   None
* Overview:		   Set the designated PIN as INPUT or OUTPUT
***********************************************************************/
static void OW_PIN_DIRECTION (DIRECTION_eType direction)
{
	Chip_GPIO_SetPinDIR(LPC_GPIO, ow.port, ow.pin, direction);
}

/**********************************************************************
* Function:        static void OW_WRITE_PIN(LOGIC_LEVEL_eType bitstate)
* PreCondition:    Pin must has being set as Output first
* Input:		   Pin logical level as High or Low
* Output:		   None
* Overview:		   Set the designated Pin as High Level or Low level
***********************************************************************/
static void OW_WRITE_PIN(LOGIC_LEVEL_eType bitstate)
{
	switch (bitstate){
		case OW_LOW:
			LPC_GPIO[ow.port].CLR = 1<<ow.pin;;
			break;
		case OW_HIGH:
			LPC_GPIO[ow.port].SET = 1<<ow.pin;
			break;
		default:
			break;
	}
}

/**********************************************************************
* Function:        static uint8_t OW_READ_PIN(void)
* PreCondition:    Pin must has being set as Input first
* Input:		   None
* Output:		   LOgical level state of the Pin
* Overview:		   Read the designated pin and return the state back to the caller
* 				   High or Low
***********************************************************************/
static uint8_t OW_READ_PIN(void)
{
	return (bool) ((LPC_GPIO[ow.port].PIN >> ow.pin) & 1);
}

//****** V A R I A B L E S ********************************************************/
unsigned char macro_delay;

/**********************************************************************
* Function:        void OW_PinInit(void)
* PreCondition:    None
* Input:		   None
* Output:		   None
* Overview:		   Configure the designated GPIO as the 1wire Pin
***********************************************************************/
void OW_PinInit(uint8_t port, uint8_t pin)
{
	// PORT 1 PIN 5 for 1 wire
	Chip_IOCON_Init(OW_IO_PINS);
	Chip_IOCON_PinMux( OW_IO_PINS , port , pin , IOCON_MODE_INACT , IOCON_FUNC0 );
	ow.port = port;
	ow.pin = pin;
}

/**********************************************************************
* Function:        void drive_OW_low (void)
* PreCondition:    None
* Input:		   None	
* Output:		   None	
* Overview:		   Configure the OW_PIN as Output and drive the OW_PIN LOW.	
***********************************************************************/
void drive_OW_low (void)
{
	OW_PIN_DIRECTION(OW_OUTPUT);
	OW_WRITE_PIN(OW_LOW);
}

/**********************************************************************
* Function:        void drive_OW_high (void)
* PreCondition:    None
* Input:		   None	
* Output:		   None	
* Overview:		   Configure the OW_PIN as Output and drive the OW_PIN HIGH.	
***********************************************************************/
static void drive_OW_high (void)
{
	OW_PIN_DIRECTION(OW_OUTPUT);
	OW_WRITE_PIN(OW_HIGH);
}

/**********************************************************************
* Function:        unsigned char read_OW (void)
* PreCondition:    None
* Input:		   None	
* Output:		   Return the status of OW pin.	
* Overview:		   Configure as Input pin and Read the status of OW_PIN 	
***********************************************************************/
static SetState read_OW (void){
	OW_PIN_DIRECTION(OW_INPUT);
	 if (OW_READ_PIN())
	 	return OW_HIGH;
	 else 	
		return OW_LOW;
}

/**********************************************************************
* Function:        unsigned char OW_reset_pulse(void)
* PreCondition:    None
* Input:		   None	
* Output:		   Return the Presense Pulse from the slave.	
* Overview:		   Initialization sequence start with reset pulse.
*				   This code generates reset sequence as per the protocol
***********************************************************************/

PRESENCE_t OW_reset_pulse(void)
{

	PRESENCE_t presence;
  	drive_OW_low(); 				// Drive the bus low
  	_delay_uS(DELAY_480Us);	  		// delay 480 microsecond (us)
	//drive_OW_high ();				// Release the bus
  	OW_PIN_DIRECTION(OW_INPUT);
	_delay_uS(DELAY_70Us);			// delay 70 microsecond (us)
	presence = read_OW();			//Sample for presence pulse from slave
	_delay_uS(DELAY_410Us);	  			// delay 200 microsecond (us)
	drive_OW_high ();		    	// Release the bus
	if (presence) return (OW_DEV_NOT_PRESENT);
		else return (OW_DEV_PRESENT);
}	

/**********************************************************************
* Function:        void OW_write_bit (unsigned char write_data)
* PreCondition:    None
* Input:		   Write a bit to 1-wire slave device.
* Output:		   None
* Overview:		   This function used to transmit a single bit to slave device.
*				   
***********************************************************************/

void OW_write_bit (uint8_t write_bit){
	if (write_bit)
	{
		//writing a bit '1'
		drive_OW_low(); 				// Drive the bus low
		_delay_uS(DELAY_6Us);		// delay 6 microsecond (us)
		drive_OW_high ();  				// Release the bus
		_delay_uS(DELAY_64Us);		// delay 64 microsecond (us)
	}
	else{
		//writing a bit '0'
		drive_OW_low(); 				// Drive the bus low
		_delay_uS(DELAY_60Us);		// delay 60 microsecond (us)
		drive_OW_high ();  				// Release the bus
		_delay_uS(DELAY_10Us);		// delay 10 microsecond for recovery (us)
	}
}	


/**********************************************************************
* Function:        unsigned char OW_read_bit (void)
* PreCondition:    None
* Input:		   None
* Output:		   Return the status of the OW PIN
* Overview:		   This function used to read a single bit from the slave device.
*				   
***********************************************************************/

uint8_t OW_read_bit (void)
{
	uint8_t read_data;
	//reading a bit 
	drive_OW_low(); 						// Drive the bus low
	_delay_uS(DELAY_6Us);				// delay 6 microsecond (us)
	drive_OW_high ();  						// Release the bus
	_delay_uS(DELAY_9Us);				// delay 9 microsecond (us)
	read_data = read_OW();					// Read the status of OW_PIN
	_delay_uS(DELAY_55Us);				// delay 55 microsecond (us)
	return read_data;
}

/**********************************************************************
* Function:        void OW_write_byte (unsigned char write_data)
* PreCondition:    None
* Input:		   Send byte to 1-wire slave device
* Output:		   None
* Overview:		   This function used to transmit a complete byte to slave device.
*				   
***********************************************************************/
void OW_write_byte (uint8_t write_data)
{
	uint8_t loop;
	for (loop = 0; loop < 8; loop++)
	{
		OW_write_bit(write_data & 0x01); 	//Sending LS-bit first
		write_data >>= 1;					// shift the data byte for the next bit to send
	}	
}	

/**********************************************************************
* Function:        unsigned char OW_read_byte (void)
* PreCondition:    None
* Input:		   None
* Output:		   Return the read byte from slave device
* Overview:		   This function used to read a complete byte from the slave device.
*				   
***********************************************************************/

uint8_t OW_read_byte (void)
{
	uint8_t loop, result=0;
	for (loop = 0; loop < 8; loop++)
	{
		result >>= 1; 				// shift the result to get it ready for the next bit to receive
		if (OW_read_bit())
		result |= 0x80;				// if result is one, then set MS-bit
	}
	return result;					
}	

/********************************************************************************************
                  E N D     O F     1 W I R E . C  
*********************************************************************************************/

