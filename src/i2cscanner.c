/*
 * i2cscanner.c
 *
 *  Created on: Aug 6, 2014
 *      Author: dsolano
 */

#include <i2cscanner.h>
#include <hardware_delay.h>
#include <monitor.h>
#include <BSP_Waveshare/bsp_waveshare.h>

#define I2C_CLOCK_FREQ		100000

#define UART_DEV			LPC_UART0
#define UART_RATE			115200

#define SPC					0x20
#define ENTER				0x0d

Status stat;
uint8_t buffer[3];
char prt_buf[96];

//*********************************************************
// Local Functions
//*********************************************************
static Status I2CWriteData(uint8_t* buffer, uint32_t len, uint32_t addr,I2C_ID_T module);
static void __putc(uint8_t c);

/* I2C Write Data*/
static Status I2CWriteData(uint8_t* buff, uint32_t len, uint32_t addr,I2C_ID_T module) {
	/* Sets data to be send to RTC to init*/
	I2C_XFER_T i2ctx;	//Data structure to be used to send byte thru I2C Master Data Transfer
	// Fill Data Structure with proper data
	i2ctx.rxBuff = 0;
	i2ctx.rxSz = 0;
	i2ctx.slaveAddr = addr;
	i2ctx.txBuff = buff;
	i2ctx.txSz = len;
	// Send data to I2C
	stat = Chip_I2C_MasterTransfer(module, &i2ctx);
	if (stat == 0)
		return (stat);
	else
		return (1);
}

//******************************
// Function to encapsulate UART
static void __putc(uint8_t c) {
	Chip_UART_SendByte(UART_DEV, c);
}

//*********************************************************
// End of Local Functions
//*********************************************************

//*********************************************************
// Public Functions
//*********************************************************


/* Function that probes all available slaves connected to an I2C bus */
void i2cscan(I2C_ID_T module)
{
	uint8_t i;
	// Init UART0 for every parameter

	xdev_out( __putc );


	wsBoard_UARTPutSTR("NXP/Waveshare OPEN1788 Cortex M3 w/LCD Controller + external RAM/Flash...\n");
	xsprintf(prt_buf, "Probing available I2C devices on LPC Bus: %d" , module);
	wsBoard_UARTPutSTR(prt_buf);
	wsBoard_UARTPutSTR("\n     00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F");
	wsBoard_UARTPutSTR("\n====================================================\n");
	for (i = 0; i <= 0x7F; i++) {
		if (!(i & 0x0F)){
			wsBoard_UARTPutSTR("\n ");
			xsprintf(prt_buf, "%2x" , i >> 4);
			wsBoard_UARTPutSTR(prt_buf);
			wsBoard_UARTPutChar(SPC);
		}
		if (i <= 7 || i > 0x78) {
			wsBoard_UARTPutSTR("   ");
			continue;
		}
		// Sets buffer to send
		buffer[0] = 1;
		stat = I2CWriteData(buffer, 1, i,module );
		if(stat == 0){
			wsBoard_UARTPutChar(SPC);
			xsprintf(prt_buf, "%2x" , i);
			wsBoard_UARTPutSTR(prt_buf);
		}
		else
			wsBoard_UARTPutSTR(" --");
		_delay_ms(1);
	}
	wsBoard_UARTPutSTR("\n");
}
