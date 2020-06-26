#ifndef _1wire_H
#define _1wire_H


/** I N C L U D E S **********************************************************/
#include <chip.h>
#include <lpc_types.h>
#include <stdint.h>

/******* G E N E R I C   D E F I N I T I O N S ************************************************/

typedef enum
{
	OW_LOW = 0,
	OW_HIGH
}LOGIC_LEVEL_eType;

typedef enum
{
	OW_INPUT = 0,
	OW_OUTPUT
}DIRECTION_eType;



/******* G E N E R I C   D E F I N I T I O N S ************************************************/

#define DELAY_6Us	6
#define DELAY_9Us	9
#define DELAY_10Us	10
#define DELAY_55Us	55
#define DELAY_60Us	60
#define DELAY_64Us	64
#define DELAY_70Us	70
#define DELAY_410Us	410			// DELAY_410Us = DELAY_205Us + DELAY_205Us#define DELAY_480Us	480			// DELAY_480Us = DELAY_240Us + DELAY_240Us // since the variable is declared as unsigned char
// To define if a One Wire device is present after the query 0 and 1
typedef enum
{
	OW_DEV_NOT_PRESENT,
	OW_DEV_PRESENT
} PRESENCE_t;


/** P R O T O T Y P E S ******************************************************/
void 	OW_write_bit(uint8_t write_data);
uint8_t OW_read_bit(void);
PRESENCE_t OW_reset_pulse(void);
void 	OW_write_byte(uint8_t write_data);
SetState OW_read_byte(void);
void 	OW_PinInit(uint8_t port, uint8_t pin);

/*****************************************************************************
 V A R I A B L E S
 ******************************************************************************/

#endif
