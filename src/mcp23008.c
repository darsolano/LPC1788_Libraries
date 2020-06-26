/*
 * File:   mcp23008.h
 * Author: dsolano
 * For PIC32MX360F512L
 * Created on August 17, 2014, 6:07 PM
 */

#include "mcp23008.h"

//*******************************************************************************
//				START OF LOCAL FUNCTIONS AND VARIABLES
//*******************************************************************************

PRIVATE uint8_t txbuff[24];
PRIVATE uint8_t rxbuff[12];
Status xStatus;
PRIVATE Status mcp23008_WriteData ( uint8_t* buffer, uint8_t len, uint8_t addr );
PRIVATE Status mcp23008_ReadData ( uint8_t* buffer, uint8_t len, uint8_t addr );

/* I2C Write Data*/
PRIVATE Status mcp23008_WriteData ( uint8_t* buffer, uint8_t len, uint8_t addr )
{
    /* Sets data to be send to MCP23008 to init*/
    I2C_XFER_T i2ctx; //Data structure to be used to send byte thru I2C Master Data Transfer

    // Fill Data Structure with proper data
    i2ctx.rxBuff = 0;
    i2ctx.rxSz = 0;
    i2ctx.slaveAddr = addr;
    i2ctx.txBuff = buffer;
    i2ctx.txSz = len;

    // Send data to I2C
    xStatus = Chip_I2C_MasterTransfer ( MCP23008_I2C, &i2ctx );
    return xStatus;
}

PRIVATE Status mcp23008_ReadData ( uint8_t* buffer, uint8_t len, uint8_t addr )
{
    /* Sets data to be from MCP23008 to init*/
	I2C_XFER_T i2crx; //Data structure to be used to send byte thru I2C Master Data Transfer

    // Fill Data Structure with proper data
    i2crx.rxBuff = buffer;
    i2crx.rxSz = len;
    i2crx.slaveAddr = addr;
    i2crx.txBuff = 0;
    i2crx.txSz = 0;

    // Send data to I2C
    xStatus = Chip_I2C_MasterTransfer ( MCP23008_I2C, &i2crx );
    return xStatus;
}
//*******************************************************************************
//				END OF LOCAL FUNCTIONS AND VARIABLES
//*******************************************************************************

//*******************************************************************************
//				START OF PUBLIC FUNCTIONS AND VARIABLES
//*******************************************************************************


Status mcp23008_WriteGPIO ( uint8_t data, uint8_t addr )
{
    txbuff[0] = GPIO;
    txbuff[1] = data;
    xStatus = mcp23008_WriteData ( txbuff, 2, addr );
    return xStatus;
}

uint8_t mcp23008_ReadGPIO ( uint8_t addr )
{
    txbuff[0] = GPIO;
    xStatus = mcp23008_WriteData ( txbuff, 1, addr );

    rxbuff[0] = 0;
    xStatus = mcp23008_ReadData ( rxbuff, 1, addr );
    return rxbuff[0];
}

Status mcp23008_SetGPIODir ( MCP23008_IODIR_REG_sType *iodircfg, uint8_t addr )
{
    txbuff[0] = IODIR;
    txbuff[1] = iodircfg->IODir_reg;
    xStatus = mcp23008_WriteData ( txbuff, 2, addr );
    return xStatus;
}

Status mcp23008_ConfigIOCON(MCP23008_IOCON_REG_sType *iocon, uint8_t addr){
    txbuff[0] = IOCON;
    txbuff[1] = iocon->IOCON_Reg;
    xStatus = mcp23008_WriteData ( txbuff, 2, addr );
    return xStatus;
}

Status mcp23008_SetGPIOPullUps ( uint8_t pullup, uint8_t addr )
{
    txbuff[0] = GPPU;
    txbuff[1] = pullup;

    xStatus = mcp23008_WriteData ( txbuff, 2, addr );
    return xStatus;

}

Status mcp23008_SetGPIOInt ( uint8_t config, uint8_t addr )
{
    txbuff[0] = DEFVAL;
    txbuff[1] = config;
    xStatus = mcp23008_WriteData ( txbuff, 2, addr );

    txbuff[0] = INTCON;
    txbuff[1] = config;
    xStatus = mcp23008_WriteData ( txbuff, 2, addr );
    mcp23008_WriteGPIO ( config, MCP23008_KP_ADDR );

    txbuff[0] = GPINTEN;
    txbuff[1] = config;
    xStatus = mcp23008_WriteData ( txbuff, 2, addr );

    return xStatus;
}

uint8_t mcp23008_ReadINTCAP ( uint8_t addr )
{
    txbuff[0] = INTCAP;
    xStatus = mcp23008_WriteData ( txbuff, 1, addr );
    rxbuff[0] = 0;
    xStatus = mcp23008_ReadData ( rxbuff, 1, MCP23008_KP_ADDR );
    return rxbuff[0];
}

uint8_t mcp23008_ReadINTF ( uint8_t addr )
{
    txbuff[0] = INTF;
    xStatus = mcp23008_WriteData ( txbuff, 1, addr );
    rxbuff[0] = 0;
    xStatus = mcp23008_ReadData ( rxbuff, 1, addr );
    return rxbuff[0];
}

Status mcp23008_SetIPOL ( uint8_t config, uint8_t addr )
{ // Polarity = 1 is inverted if 0 output = 1 and viceversa
    txbuff[0] = IPOL;
    txbuff[1] = config; // bitwise for set up all bits from 0 to 7

    return mcp23008_WriteData ( txbuff, 2, addr );
}

uint8_t mcp23008_ReadOLAT ( uint8_t addr )
{
    txbuff[0] = OLAT;
    xStatus = mcp23008_WriteData ( txbuff, 1, addr );
    rxbuff[0] = 0;
    xStatus = mcp23008_ReadData ( rxbuff, 1, addr );
    return rxbuff[0];
}

Status mcp23008_WriteOLAT ( uint8_t data, uint8_t addr )
{ // Polarity = 1 is inverted if 0 output = 1 and viceversa
    txbuff[0] = OLAT;
    txbuff[1] = data; // bitwise for set up all bits from 0 to 7

    return mcp23008_WriteData ( txbuff, 2, addr );
}

//*******************************************************************************
//				END OF PUBLIC FUNCTIONS AND VARIABLES
//*******************************************************************************
