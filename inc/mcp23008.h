/* 
 * File:   mcp23008.h
 * Author: dsolano
 * For NXP LPC-1769 Cortex M3
 * Created on August 17, 2014, 6:07 PM
 * Breakout Board with LCD1602 Drived by MCP23008 #1 and Keypad Drived by MCP23008 #2
 * with a T18b20 temp sensor 1 wire
 */

#ifndef MCP23008_H
#define	MCP23008_H

#ifdef	__cplusplus
extern "C"
{
#endif

#include <chip.h>
#include <lpc_types.h>



#define PRIVATE						static
//#define MCP23008_LCD_ADDR           0x20    // For LCD1602
#define MCP23008_KP_ADDR            0x21    // For Keypad 4x4
#define MCP23008_I2C_BUS            LPC_I2C2
#define MCP23008_I2C				I2C2


//Address Access to MCP23008 Internal registers
#define IODIR                      0x00 // Used
#define IPOL                       0x01 // Used
#define GPINTEN                    0x02 // Not Used
#define DEFVAL                     0x03 // Not Used
#define INTCON                     0x04 // Not Used
#define IOCON                      0x05 // Used
#define GPPU                       0x06 // Used
#define INTF                       0x07 // Not Used
#define INTCAP                     0x08  //(Read-only)
#define GPIO                       0x09 // Used
#define OLAT                       0x0A // Used

// IODIR – I/O DIRECTION REGISTER (ADDR 0x00)

typedef enum
{
	output, input
} MCP23008_IODIR_eType;

typedef union
{

	struct
	{
		unsigned IO0 :1;
		unsigned IO1 :1;
		unsigned IO2 :1;
		unsigned IO3 :1;
		unsigned IO4 :1;
		unsigned IO5 :1;
		unsigned IO6 :1;
		unsigned IO7 :1;
	};
	uint8_t IODir_reg;
} MCP23008_IODIR_REG_sType;


// IPOL – INPUT POLARITY PORT REGISTER (ADDR 0x01)

typedef enum
{
	Normal_Polarity, Inverted_Polarity
} MCP23008_IPOL_eType;

typedef union
{

	struct
	{
		unsigned IPOL0 :1;
		unsigned IPOL1 :1;
		unsigned IPOL2 :1;
		unsigned IPOL3 :1;
		unsigned IPOL4 :1;
		unsigned IPOL5 :1;
		unsigned IPOL6 :1;
		unsigned IPOL7 :1;
	};
	uint8_t IPOL_Reg;
} MCP23008_IPOL_REG_sType;

// IOCON – I/O EXPANDER CONFIGURATION REGISTER (ADDR 0x05)

typedef union
{

	struct
	{
		unsigned :1;
		unsigned INTPOL :1;
		unsigned ODR :1;
		unsigned HAEN :1;
		unsigned DISSLW :1;
		unsigned SEQOP :1;
		unsigned :2;
	};
	uint8_t IOCON_Reg;
} MCP23008_IOCON_REG_sType;

// GPPU – GPIO PULL-UP RESISTOR REGISTER (ADDR 0x06)

typedef enum
{
	PullUp_Disable, PullUp_Enable
} MCP23008_GPPU_eType;

typedef union
{

	struct
	{
		unsigned PU0 :1;
		unsigned PU1 :1;
		unsigned PU2 :1;
		unsigned PU3 :1;
		unsigned PU4 :1;
		unsigned PU5 :1;
		unsigned PU6 :1;
		unsigned PU7 :1;
	};
	uint8_t GPPU_Reg;
} MCP23008_GPPU_REG_sType;

// GPIO – GENERAL PURPOSE I/O PORT REGISTER (ADDR 0x09)

typedef enum
{
	Logic_Low, Logic_High
} MCP23008_GPIO_eType;

typedef union
{

	struct
	{
		unsigned GP0 :1;
		unsigned GP1 :1;
		unsigned GP2 :1;
		unsigned GP3 :1;
		unsigned GP4 :1;
		unsigned GP5 :1;
		unsigned GP6 :1;
		unsigned GP7 :1;
	};
	uint8_t GPIO_Reg;
} MCP23008_GPIO_REG_sType;

// OLAT – OUTPUT LATCH REGISTER 0 (ADDR 0x0A)

typedef union
{

	struct
	{
		unsigned OL0 :1;
		unsigned OL1 :1;
		unsigned OL2 :1;
		unsigned OL3 :1;
		unsigned OL4 :1;
		unsigned OL5 :1;
		unsigned OL6 :1;
		unsigned OL7 :1;
	};
	uint8_t OLAT_Reg;
} MCP23008_OLAT_REG_sType;

typedef union
{
	struct
	{
		unsigned IOC0 :1;
		unsigned IOC1 :1;
		unsigned IOC2 :1;
		unsigned IOC3 :1;
		unsigned IOC4 :1;
		unsigned IOC5 :1;
		unsigned IOC6 :1;
		unsigned IOC7 :1;
	};
	uint8_t INTCON_Reg;
} MCP23008_INTCON_REG_sType;


typedef union
{
	struct
	{
		unsigned DEF0 :1;
		unsigned DEF1 :1;
		unsigned DEF2 :1;
		unsigned DEF3 :1;
		unsigned DEF4 :1;
		unsigned DEF5 :1;
		unsigned DEF6 :1;
		unsigned DEF7 :1;
	};
	uint8_t DEFVAL_Reg;
} MCP23008_DEFVAL_REG_sType;

typedef union
{
	struct
	{
		unsigned GPINT0 :1;
		unsigned GPINT1 :1;
		unsigned GPINT2 :1;
		unsigned GPINT3 :1;
		unsigned GPINT4 :1;
		unsigned GPINT5 :1;
		unsigned GPINT6 :1;
		unsigned GPINT7 :1;
	};
	uint8_t GPINTEN_Reg;
} MCP23008_GPINTEN_REG_sType;


Status  mcp23008_WriteGPIO(uint8_t data, uint8_t addr);
uint8_t mcp23008_ReadGPIO(uint8_t addr);
Status  mcp23008_SetGPIODir(MCP23008_IODIR_REG_sType *iodircfg,uint8_t addr);
Status  mcp23008_SetGPIOPullUps(uint8_t pullup, uint8_t addr);
Status  mcp23008_SetGPIOInt(uint8_t config, uint8_t addr);
uint8_t mcp23008_ReadINTCAP(uint8_t addr);
uint8_t mcp23008_ReadINTF(uint8_t addr);
Status  mcp23008_SetIPOL(uint8_t config, uint8_t addr);
Status  mcp23008_WriteOLAT(uint8_t data, uint8_t addr);
uint8_t mcp23008_ReadOLAT ( uint8_t addr );
Status  mcp23008_WriteOLAT ( uint8_t data, uint8_t addr );
Status  mcp23008_ConfigIOCON(MCP23008_IOCON_REG_sType *iocon, uint8_t addr);


#ifdef	__cplusplus
}
#endif

#endif	/* MCP23008_H */

