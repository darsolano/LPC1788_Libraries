/*
 * serial_3w.c
 *
 *  Created on: Dec 4, 2015
 *      Author: dsolano
 */

#include <serial_3w.h>
#include <hardware_delay.h>
#include <define_pins.h>
/*
 * CS  - PIN 1.18
 * CLK - PIN 1.19
 * DOUT - PIN 1.20
 */
DEFINE_PIN(shift_CLK,1,30 )
DEFINE_PIN(shift_CS,1,31)
DEFINE_PIN(shift_DOUT,2,12)



void serial3wire_init(void)
{
	Chip_IOCON_Init(LPC_IOCON);
	Chip_GPIO_Init(LPC_GPIO);
	// Set DIRECTION for Pins
	shift_CS_OUTPUT();
	shift_CLK_OUTPUT();
	shift_DOUT_OUTPUT();
	// Initial state
	shift_CS_HIGH();
	shift_CLK_LOW();
	shift_DOUT_LOW();
}

// SPI BitBang to send data 8 bits
void serial3wire_write_byte(uint8_t data)
{
	uint8_t temp , count;

	temp=data;
	shift_CS_SELECTED();
	shift_CLK_LOW();
	for( count=0; count<8; count++)
	{
		if(temp & 0x80)
			shift_DOUT_HIGH();
		else
			shift_DOUT_LOW();
		temp <<=  1;
		// pseudo clock
		shift_CLK_HIGH();
		__NOP();
		shift_CLK_LOW();
		__NOP();
	}
	shift_CS_DESELECTED();
}

// SPI BitBang to send data 16 bits
void serial3wire_write_short(uint16_t data)
{
	uint16_t temp , count;

	temp=data;
	shift_CS_SELECTED();
	shift_CLK_LOW();
	for( count=0; count<16; count++)
	{
		if(temp & 0x8000)
			shift_DOUT_HIGH();
		else
			shift_DOUT_HIGH();
		temp <<=  1;
		// pseudo clock
		shift_CLK_HIGH();
		__NOP();
		shift_CLK_LOW();
		__NOP();
	}
	shift_CS_DESELECTED();
}

// SPI BitBang to send data 32 bits
void serial3wire_write_word(uint32_t data)
{
	uint16_t temp , count;

	temp=data;
	shift_CS_SELECTED();
	shift_CLK_LOW();
	for( count=0; count<32; count++)
	{
		if(temp & 0x80000000)
			shift_DOUT_HIGH();
		else
			shift_DOUT_LOW();
		temp <<=  1;
		// pseudo clock
		shift_CLK_HIGH();
		__NOP();
		shift_CLK_LOW();
		__NOP();
	}
	shift_CS_DESELECTED();
}

