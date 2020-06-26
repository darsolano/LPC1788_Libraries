/*
  ITDB02_Graph16.cpp - Arduino library support for 16bit (W)QVGA LCD Boards
  Copyright (C)2010-2011 Henning Karlsen. All right reserved

  Basic functionality of this library are based on the demo-code provided by
  ITead studio. You can find the latest version of the library at
  http://www.henningkarlsen.com/electronics

  This library was originally made especially for the 3.2" TFT LCD Screen Module:
  ITDB02-3.2 by ITead studio, but has later been expanded to support multiple
  modules. This library has been designed to use 16bit mode, so it will not
  work with 8bit modules.

  Supported controllers:
		-	HX8347-A
		-	ILI9325D
		-	ILI9327
		-	SSD1289

  If you make any modifications or improvements to the code, I would appreciate
  that you share the code with me so that I might include it in the next release.
  I can be contacted through http://www.henningkarlsen.com/electronics/contact.php

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <ssd1289.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <define_pins.h>
#include <BSP_Waveshare/bsp_waveshare.h>


#define tft32Xmit				LPC_SSP0

DEFINE_PIN(tft32_RS,1,18)
DEFINE_PIN(tft32_WR,1,19)
DEFINE_PIN(tft32_RD,1,20)
DEFINE_PIN(tft32_CS,1,21)
DEFINE_PIN(tft32_REST,1,22)
DEFINE_PIN(tft32_LED_A,1,23)

DEFINE_PIN(shift_CS,0,16)



/*
 * Private functions
 */
static void _delay_ms (uint16_t ms);



DISPLAY_t *ptft = &tft;

static void _delay_ms (uint16_t ms)
{
	uint16_t delay;
	volatile uint32_t i;
	for (delay = ms; delay >0 ; delay--)
	{
		for (i=3500; i >0;i--){};//1ms loop with -Os optimisation
	}
}

uint8_t SpiIO(LPC_SSP_T* SSPdev, uint8_t data)
{
	while (!(SSPdev->SR & SSP_STAT_TFE));
	SSPdev->DR = data;		// send a byte
	while (!(SSPdev->SR & SSP_STAT_RNE));
	return SSPdev->DR;		// Receive a byte
}
void tft32_backlight_on(void)
{
	tft32_LED_A_HIGH();
}

void tft32_backlight_off(void)
{
	tft32_LED_A_LOW();
}

void tft32_hdw_reset(void){
	tft32_REST_LOW();
	_delay_ms(50);
	tft32_REST_HIGH();
	_delay_ms(15);
	tft32_RD_HIGH();
}

void tft32_PINS(void)
{
	Chip_IOCON_Init(LPC_IOCON);
	Chip_GPIO_Init(LPC_GPIO1);


	tft32_RS_OUTPUT();
	tft32_WR_OUTPUT();
	tft32_RD_OUTPUT();
	tft32_CS_OUTPUT();
	tft32_REST_OUTPUT();
	tft32_LED_A_OUTPUT();
	//SPI Setup
	shift_CS_OUTPUT();

//	Board_SSP_Init(tft32Xmit);
//	Chip_Clock_SetPCLKDiv(SYSCTL_PCLK_SSP0 , SYSCTL_CLKDIV_2);
//	Chip_SSP_Init(tft32Xmit);
//	Chip_SSP_SetFormat(tft32Xmit, SSP_BITS_8, SSP_FRAMEFORMAT_SPI, SSP_CLOCK_CPHA0_CPOL0);
//	Chip_SSP_SetClockRate(tft32Xmit,1,2);
//	Chip_SSP_Enable(tft32Xmit);
}

void tft32_LCD_Writ_Bus(uint16_t frame)
{
	tft32_CS_SELECTED();
	tft32_WR_LOW();
	//Start of SPI Xfer
	shift_CS_SELECTED();
	SpiIO(tft32Xmit,(frame&0xff00)>>8);
	SpiIO(tft32Xmit,frame&0x00ff);
	shift_CS_DESELECTED();
	//End of SPI Xfer
	tft32_WR_HIGH();
	tft32_CS_DESELECTED();
}

void tft32_LCD_Write_COM(uint16_t cmd)
{
	tft32_RS_LOW();
	tft32_LCD_Writ_Bus(cmd);
}

void tft32_LCD_Write_DATA(uint16_t data)
{
	tft32_RS_HIGH();
	tft32_LCD_Writ_Bus(data);
}

void tft32_main_W_com_data(uint16_t cmd,uint16_t data)
{
	tft32_LCD_Write_COM(cmd);
	tft32_LCD_Write_DATA(data);
}

void tft32_set_orientation(byte orientation)
{
	ptft->orient=orientation;
	ptft->disp_x_size = X_SIZE-1;
	ptft->disp_y_size = Y_SIZE-1;
}

void tft32_DisplayON(void)
{
	// Power supply setting
	tft32_main_W_com_data(POWER_CONTROL_1, 0xA8A4);
	tft32_main_W_com_data(POWER_CONTROL_2, 0x0000);
	tft32_main_W_com_data(POWER_CONTROL_3, 0x000C);
	tft32_main_W_com_data(POWER_CONTROL_4, 0x2B00);
	tft32_main_W_com_data(POWER_CONTROL_5, 0x00B7);

	tft32_main_W_com_data(DISPLAY_CONTROL, 0x0021);

	tft32_main_W_com_data(OSCILLATOR_START, 0x0001);

	tft32_main_W_com_data(DISPLAY_CONTROL, 0x0023);

	// Exit sleep mode
	tft32_main_W_com_data(SLEEP_MODE, 0x0000);
	_delay_ms(30);

	tft32_main_W_com_data(DISPLAY_CONTROL, 0x0033);

	// Entry mode setting
	tft32_main_W_com_data(ENTRY_MODE, 0x6070);

	// LCD driver AC setting
	tft32_main_W_com_data(LCD_DRIVER_AC_CONTROL, 0x0600);

	tft32_main_W_com_data(DRIVER_OUTPUT_CONTROL, 0x2B3F);
}

void tft32_SleepMode(void)
{
	tft32_main_W_com_data(SLEEP_MODE, 0x0001);
}

void tft32_ReleaseFromSleep(void)
{
	tft32_main_W_com_data(SLEEP_MODE, 0x0000);
}


void tft32_InitLCD(void)
{
/*
	tft32_main_W_com_data(0x00,0x0001);
	tft32_main_W_com_data(0x03,0xA8A4);
	tft32_main_W_com_data(0x0C,0x0000);
	tft32_main_W_com_data(0x0D,0x080C);
	tft32_main_W_com_data(0x0E,0x2B00);
	tft32_main_W_com_data(0x1E,0x00B7);
	tft32_main_W_com_data(0x01,0x2B3F);
	tft32_main_W_com_data(0x02,0x0600);
	tft32_main_W_com_data(0x10,0x0000);
	tft32_main_W_com_data(0x11,0x6070);
	tft32_main_W_com_data(0x05,0x0000);
	tft32_main_W_com_data(0x06,0x0000);
	tft32_main_W_com_data(0x16,0xEF1C);
	tft32_main_W_com_data(0x17,0x0003);
	tft32_main_W_com_data(0x07,0x0233);
	tft32_main_W_com_data(0x0B,0x0000);
	tft32_main_W_com_data(0x0F,0x0000);
	tft32_main_W_com_data(0x41,0x0000);
	tft32_main_W_com_data(0x42,0x0000);
	tft32_main_W_com_data(0x48,0x0000);
	tft32_main_W_com_data(0x49,0x013F);
	tft32_main_W_com_data(0x4A,0x0000);
	tft32_main_W_com_data(0x4B,0x0000);
	tft32_main_W_com_data(0x44,0xEF00);
	tft32_main_W_com_data(0x45,0x0000);
	tft32_main_W_com_data(0x46,0x013F);
	tft32_main_W_com_data(0x30,0x0707);
	tft32_main_W_com_data(0x31,0x0204);
	tft32_main_W_com_data(0x32,0x0204);
	tft32_main_W_com_data(0x33,0x0502);
	tft32_main_W_com_data(0x34,0x0507);
	tft32_main_W_com_data(0x35,0x0204);
	tft32_main_W_com_data(0x36,0x0204);
	tft32_main_W_com_data(0x37,0x0502);
	tft32_main_W_com_data(0x3A,0x0302);
	tft32_main_W_com_data(0x3B,0x0302);
	tft32_main_W_com_data(0x23,0x0000);
	tft32_main_W_com_data(0x24,0x0000);
	tft32_main_W_com_data(0x25,0x8000);
	tft32_main_W_com_data(0x4f,0x0000);
	tft32_main_W_com_data(0x4e,0x0000);
	tft32_LCD_Write_COM(0x22);
*/
	  tft32_main_W_com_data (0x00,0x0001);
	  _delay_ms(20);
	  tft32_main_W_com_data (0x03,0xA2A4);
	  tft32_main_W_com_data (0x0C,0x0004);
	  tft32_main_W_com_data (0x0D,0x0308);
	  tft32_main_W_com_data (0x0E,0x3000);
	  _delay_ms(50);
	  tft32_main_W_com_data (0x1E,0x00AF);
	  tft32_main_W_com_data (0x01,0x2B3F);
	  tft32_main_W_com_data (0x02,0x0600);
	  tft32_main_W_com_data (0x10,0x0000);
	  tft32_main_W_com_data (0x07,0x0233);
	  tft32_main_W_com_data (0x0B,0x0039);
	  tft32_main_W_com_data (0x0F,0x0000);
	  _delay_ms(50);

	  tft32_main_W_com_data (0x30,0x0707);
	  tft32_main_W_com_data (0x31,0x0204);
	  tft32_main_W_com_data (0x32,0x0204);
	  tft32_main_W_com_data (0x33,0x0502);
	  tft32_main_W_com_data (0x34,0x0507);
	  tft32_main_W_com_data (0x35,0x0204);
	  tft32_main_W_com_data (0x36,0x0204);
	  tft32_main_W_com_data (0x37,0x0502);
	  tft32_main_W_com_data (0x3A,0x0302);
	  tft32_main_W_com_data (0x3B,0x0302);

	  tft32_main_W_com_data (0x23,0x0000);
	  tft32_main_W_com_data (0x24,0x0000);

	  tft32_main_W_com_data (0x48,0x0000);
	  tft32_main_W_com_data (0x49,0x013F);
	  tft32_main_W_com_data (0x4A,0x0000);
	  tft32_main_W_com_data (0x4B,0x0000);

	  tft32_main_W_com_data (0x41,0x0000);
	  tft32_main_W_com_data (0x42,0x0000);

	  tft32_main_W_com_data (0x44,0xEF00);
	  tft32_main_W_com_data (0x45,0x0000);
	  tft32_main_W_com_data (0x46,0x013F);
	  _delay_ms(50);

	  tft32_main_W_com_data (0x44,0xEF00);
	  tft32_main_W_com_data (0x45,0x0000);
	  tft32_main_W_com_data (0x4E,0x0000);
	  tft32_main_W_com_data (0x4F,0x0000);
	  tft32_main_W_com_data (0x46,0x013F);
}

void tft32_setXY(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	int tmp;

	if (ptft->orient==LANDSCAPE)
	{
		tmp=x1;
		x1=y1;
		y1=ptft->disp_y_size-tmp;
		tmp=x2;
		x2=y2;
		y2=ptft->disp_y_size-tmp;
		tmp=y1;
		y1=y2;
		y2=tmp;
	}

	tft32_main_W_com_data(HORIZONTAL_RAM_ADDR_POS,(x2<<8)+x1);
	tft32_main_W_com_data(VERTICAL_RAM_ADDR_START_POS,y1);
	tft32_main_W_com_data(VERTICAL_RAM_ADDR_END_POS,y2);
	tft32_main_W_com_data(SET_GDDRAM_X_ADDR_COUNTER,x1);
	tft32_main_W_com_data(SET_GDDRAM_Y_ADDR_COUNTER,y1);
	tft32_LCD_Write_COM(RAM_DATA_WRITE);
}

void tft32_drawRect(int x1, int y1, int x2, int y2)
{
	int tmp;

	if (x1>x2)
	{
		tmp=x1;
		x1=x2;
		x2=tmp;
	}
	if (y1>y2)
	{
		tmp=y1;
		y1=y2;
		y2=tmp;
	}

	tft32_drawHLine(x1, y1, x2-x1);
	tft32_drawHLine(x1, y2, x2-x1);
	tft32_drawVLine(x1, y1, y2-y1);
	tft32_drawVLine(x2, y1, y2-y1);
}

void tft32_drawRoundRect(int x1, int y1, int x2, int y2)
{
	int tmp;

	if (x1>x2)
	{
		tmp=x1;
		x1=x2;
		x2=tmp;
	}
	if (y1>y2)
	{
		tmp=y1;
		y1=y2;
		y2=tmp;
	}
	if ((x2-x1)>4 && (y2-y1)>4)
	{
		tft32_drawPixel(x1+1,y1+1);
		tft32_drawPixel(x2-1,y1+1);
		tft32_drawPixel(x1+1,y2-1);
		tft32_drawPixel(x2-1,y2-1);
		tft32_drawHLine(x1+2, y1, x2-x1-4);
		tft32_drawHLine(x1+2, y2, x2-x1-4);
		tft32_drawVLine(x1, y1+2, y2-y1-4);
		tft32_drawVLine(x2, y1+2, y2-y1-4);
	}
}

void tft32_fillRect(int x1, int y1, int x2, int y2)
{
	int tmp , i;

	if (x1>x2)
	{
		tmp=x1;
		x1=x2;
		x2=tmp;
	}
	if (y1>y2)
	{
		tmp=y1;
		y1=y2;
		y2=tmp;
	}

	if (ptft->orient==PORTRAIT)
	{
		for ( i=0; i<((y2-y1)/2)+1; i++)
		{
			tft32_drawHLine(x1, y1+i, x2-x1);
			tft32_drawHLine(x1, y2-i, x2-x1);
		}
	}
	else
	{
		for ( i=0; i<((x2-x1)/2)+1; i++)
		{
			tft32_drawVLine(x1+i, y1, y2-y1);
			tft32_drawVLine(x2-i, y1, y2-y1);
		}
	}
}

void tft32_fillRoundRect(int x1, int y1, int x2, int y2)
{
	int tmp , i;

	if (x1>x2)
	{
		tmp=x1;
		x1=x2;
		x2=tmp;
	}
	if (y1>y2)
	{
		tmp=y1;
		y1=y2;
		y2=tmp;
	}

	if ((x2-x1)>4 && (y2-y1)>4)
	{
		for ( i=0; i<((y2-y1)/2)+1; i++)
		{
			switch(i)
			{
			case 0:
				tft32_drawHLine(x1+2, y1+i, x2-x1-4);
				tft32_drawHLine(x1+2, y2-i, x2-x1-4);
				break;
			case 1:
				tft32_drawHLine(x1+1, y1+i, x2-x1-2);
				tft32_drawHLine(x1+1, y2-i, x2-x1-2);
				break;
			default:
				tft32_drawHLine(x1, y1+i, x2-x1);
				tft32_drawHLine(x1, y2-i, x2-x1);
			}
		}
	}
}

void tft32_drawCircle(int x, int y, int radius)
{
	int f = 1 - radius;
	int ddF_x = 1;
	int ddF_y = -2 * radius;
	int x1 = 0;
	int y1 = radius;


	tft32_setXY(x, y + radius, x, y + radius);
	tft32_LCD_Write_DATA(tft.fgcolor);
	tft32_setXY(x, y - radius, x, y - radius);
	tft32_LCD_Write_DATA(tft.fgcolor);
	tft32_setXY(x + radius, y, x + radius, y);
	tft32_LCD_Write_DATA(tft.fgcolor);
	tft32_setXY(x - radius, y, x - radius, y);
	tft32_LCD_Write_DATA(tft.fgcolor);

	while(x1 < y1)
	{
		if(f >= 0)
		{
			y1--;
			ddF_y += 2;
			f += ddF_y;
		}
		x1++;
		ddF_x += 2;
		f += ddF_x;
		tft32_setXY(x + x1, y + y1, x + x1, y + y1);
		tft32_LCD_Write_DATA(tft.fgcolor);
		tft32_setXY(x - x1, y + y1, x - x1, y + y1);
		tft32_LCD_Write_DATA(tft.fgcolor);
		tft32_setXY(x + x1, y - y1, x + x1, y - y1);
		tft32_LCD_Write_DATA(tft.fgcolor);
		tft32_setXY(x - x1, y - y1, x - x1, y - y1);
		tft32_LCD_Write_DATA(tft.fgcolor);
		tft32_setXY(x + y1, y + x1, x + y1, y + x1);
		tft32_LCD_Write_DATA(tft.fgcolor);
		tft32_setXY(x - y1, y + x1, x - y1, y + x1);
		tft32_LCD_Write_DATA(tft.fgcolor);
		tft32_setXY(x + y1, y - x1, x + y1, y - x1);
		tft32_LCD_Write_DATA(tft.fgcolor);
		tft32_setXY(x - y1, y - x1, x - y1, y - x1);
		tft32_LCD_Write_DATA(tft.fgcolor);
	}
	if (ptft->orient==PORTRAIT)
		tft32_setXY(0,0,ptft->disp_x_size,ptft->disp_y_size);
	else
		tft32_setXY(0,0,ptft->disp_y_size,ptft->disp_x_size);
}

void tft32_fillCircle(int x, int y, int radius)
{
	int x1, y1;
	for( y1=-radius; y1<=radius; y1++)
		for( x1=-radius; x1<=radius; x1++)
			if(x1*x1+y1*y1 <= radius*radius)
			{
				tft32_setXY(x+x1, y+y1, x+x1, y+y1);
				tft32_LCD_Write_DATA(tft.fgcolor);
			}
	if (ptft->orient==PORTRAIT)
		tft32_setXY(0,0,ptft->disp_x_size,ptft->disp_y_size);
	else
		tft32_setXY(0,0,ptft->disp_y_size,ptft->disp_x_size);
}

void tft32_clrScr(uint16_t color)	// RGB565 code
{
	long i;

	if (ptft->orient==PORTRAIT)
		tft32_setXY(0,0,ptft->disp_x_size , ptft->disp_y_size);
	else
		tft32_setXY(0,0, ptft->disp_y_size,ptft->disp_x_size);

	for (i=0; i<((ptft->disp_x_size+1)*(ptft->disp_y_size+1)); i++)
	{
		tft32_LCD_Write_DATA(color);
	}
}

//void tft32_fillScr(byte r, byte g, byte b)
void tft32_fillScr(uint16_t color)
{
	long i;

	if (ptft->orient==PORTRAIT)
		tft32_setXY(0,0,ptft->disp_x_size,ptft->disp_y_size);
	else
		tft32_setXY(0,0,ptft->disp_y_size,ptft->disp_x_size);
	for (i=0; i<(X_SIZE*(ptft->disp_y_size+1)); i++)
	{
		tft32_LCD_Write_DATA(color);
	}
}

void tft32_setColor(uint16_t color)
{
	ptft->fgcolor = color;
}

void tft32_setBackColor(uint16_t color)
{
	ptft->bgcolor = color;
}

void tft32_setPixel(uint16_t color)
{
	tft32_LCD_Write_DATA(color);
	//tft32_LCD_Write_DATA(ASSEMBLE_RGB(r,g,b));
	//tft32_LCD_Write_DATA((((r&248)|(g>>5))<<8) | (((g&28)<<3)|(b>>3)));
}

void tft32_drawPixel(int x, int y)
{
	tft32_setXY(x, y, x, y);
	tft32_setPixel(ptft->fgcolor);
	if (ptft->orient==PORTRAIT)
		tft32_setXY(0,0,ptft->disp_x_size,ptft->disp_y_size);
	else
		tft32_setXY(0,0,ptft->disp_y_size,ptft->disp_x_size);
}

void tft32_drawLine(int x1, int y1, int x2, int y2)
{
	int tmp , i;
	double delta, tx, ty;
	//double m, b, dx, dy;

	if (((x2-x1)<0))
	{
		tmp=x1;
		x1=x2;
		x2=tmp;
		tmp=y1;
		y1=y2;
		y2=tmp;
	}
    if (((y2-y1)<0))
	{
		tmp=x1;
		x1=x2;
		x2=tmp;
		tmp=y1;
		y1=y2;
		y2=tmp;
	}

	if (y1==y2)
	{
		if (x1>x2)
		{
			tmp=x1;
			x1=x2;
			x2=tmp;
		}
		tft32_drawHLine(x1, y1, x2-x1);
	}
	else if (x1==x2)
	{
		if (y1>y2)
		{
			tmp=y1;
			y1=y2;
			y2=tmp;
		}
		tft32_drawVLine(x1, y1, y2-y1);
	}
	else if (abs(x2-x1)>abs(y2-y1))
	{
		delta=((double)(y2-y1)/(double)(x2-x1));
		ty=(double)(y1);
		if (x1>x2)
		{
			for ( i=x1; i>=x2; i--)
			{
				tft32_setXY(i, (int)(ty+0.5), i, (int)(ty+0.5));
				tft32_LCD_Write_DATA(ptft->fgcolor);
        		ty=ty-delta;
			}
		}
		else
		{
			for ( i=x1; i<=x2; i++)
			{
				tft32_setXY(i, (int)(ty+0.5), i, (int)(ty+0.5));
				tft32_LCD_Write_DATA(ptft->fgcolor);
        		ty=ty+delta;
			}
		}
	}
	else
	{
		delta=((float)(x2-x1)/(float)(y2-y1));
		tx=(float)(x1);
        if (y1>y2)
        {
			for ( i=y2+1; i>y1; i--)
			{
				tft32_setXY((int)(tx+0.5), i, (int)(tx+0.5), i);
				tft32_LCD_Write_DATA(ptft->fgcolor);
        		tx=tx+delta;
			}
        }
        else
        {
			for ( i=y1; i<y2+1; i++)
			{
				tft32_setXY((int)(tx+0.5), i, (int)(tx+0.5), i);
				tft32_LCD_Write_DATA(ptft->fgcolor);
        		tx=tx+delta;
			}
        }
	}

	if (ptft->orient==PORTRAIT)
		tft32_setXY(0,0,ptft->disp_x_size,ptft->disp_y_size);
	else
		tft32_setXY(0,0,ptft->disp_y_size,ptft->disp_x_size);
}

void tft32_drawHLine(int x, int y, int l)
{
	int i;

	tft32_setXY(x, y, x+l, y);
	for ( i=0; i<l+1; i++)
	{
		tft32_LCD_Write_DATA(ptft->fgcolor);
	}
}

void tft32_drawVLine(int x, int y, int l)
{
	int i;

	tft32_setXY(x, y, x, y+l);
	for ( i=0; i<l; i++)
	{
		tft32_LCD_Write_DATA(ptft->fgcolor);
	}
}

void tft32_printChar(byte c, int x, int y)
{
	byte i,ch,n;
	uint16_t j;
	int zz,temp;
	uint8_t* ptemp;

	if (ptft->orient==PORTRAIT)
	{
		tft32_setXY(x,y,x+cfont.x_size-1,y+cfont.y_size-1);
		ptemp = (cfont.font + ((c - cfont.offset) * cfont.y_size * cfont.Hrows));

		for(j=0;j<(cfont.y_size);j++)
		{
			for(i=0;i<cfont.Hrows;i++)
			{
				ch = *(ptemp + j*cfont.Hrows + i);
				for (n=0;n<8;n++)
					if((ch&(1<<(7-n)))!=0)
					{
						tft32_setPixel(ptft->fgcolor);
					}
					else
					{
						tft32_setPixel(ptft->bgcolor);
					}
			}
		}
	}
	else
	{
		//temp = (cfont.font + ((c - cfont.offset) * cfont.y_size * cfont.Hrows));
		temp=((c-cfont.offset)*((cfont.x_size/8)*cfont.y_size));
		for(j=0;j<((cfont.x_size/8)*cfont.y_size);j+=(cfont.x_size/8))
		{
			tft32_setXY(x,y+(j/(cfont.x_size/8)),x+cfont.x_size-1,y+(j/(cfont.x_size/8)));
			for ( zz=(cfont.x_size/8)-1; zz>=0; zz--)
			{
				ch=cfont.font[temp+zz];
				for(i=0;i<8;i++)
				{
					if((ch&(1<<i))!=0)
					{
						tft32_setPixel(ptft->fgcolor);
					}
					else
					{
						tft32_setPixel(ptft->bgcolor);
					}
				}
			}
			temp+=(cfont.x_size/8);
		}
	}
}

void tft32_rotateChar(byte c, int x, int y, int pos, int deg)
{
	byte i,j,ch;
	uint16_t temp;
	int newx,newy , zz;
	double radian;
	radian=deg*0.0175;

	temp=((c-cfont.offset)*((cfont.x_size/8)*cfont.y_size));
	for(j=0;j<cfont.y_size;j++)
	{
		for ( zz=0; zz<(cfont.x_size/8); zz++)
		{
			ch=cfont.font[temp+zz];
			for(i=0;i<8;i++)
			{
				newx=x+(((i+(zz*8)+(pos*cfont.x_size))*cos(radian))-((j)*sin(radian)));
				newy=y+(((j)*cos(radian))+((i+(zz*8)+(pos*cfont.x_size))*sin(radian)));

				tft32_setXY(newx,newy,newx+1,newy+1);

				if((ch&(1<<(7-i)))!=0)
				{
					tft32_setPixel(ptft->fgcolor);
				}
				else
				{
					tft32_setPixel(ptft->bgcolor);
				}
			}
		}
		temp+=(cfont.x_size/8);
	}
}

/*
 * Print a string buffer terminated in NULL or 0
 */
void tft32_print(char *st, int x, int y, int deg)
{
	int stl, i;

	stl = strlen(st);

	if (ptft->orient==PORTRAIT)
	{
	if (x==RIGHT)
		x=240-(stl*cfont.x_size);
	if (x==CENTER)
		x=(240-(stl*cfont.x_size))/2;
	}
	else
	{
	if (x==RIGHT)
		x=(ptft->disp_y_size+1)-(stl*cfont.x_size);
	if (x==CENTER)
		x=((ptft->disp_y_size+1)-(stl*cfont.x_size))/2;
	}

	for (i=0; i<stl; i++)
		if (deg==0)
			tft32_printChar(*st++, x + (i*(cfont.x_size)), y);
		else
			tft32_rotateChar(*st++, x, y, i, deg);
}

void tft32_printNumI(long num, int x, int y)
{
  char buf[25];
  char st[27];
  Bool neg=FALSE;
  int i , c=0;

  if (num==0)
  {
	  st[0]=48;
	  st[1]=0;
  }
  else
  {
	  if (num<0)
	  {
		neg=true;
		num=-num;
	  }

	  while (num>0)
	  {
		buf[c]=48+(num % 10);
		c++;
		num=(num-(num % 10))/10;
	  }
	  buf[c]=0;

	  if (neg)
	  {
		st[0]=45;
	  }

	  for ( i=0; i<c; i++)
	  {
		st[i+neg]=buf[c-i-1];
	  }
	  st[c+neg]=0;
  }

  tft32_print(st,x,y,0);
}

void tft32_printNumF(double num, byte dec, int x, int y)
{
  char buf[25];
  char st[27];
  Bool neg=FALSE;
  int c=0;
  int c2, i;
  unsigned long inum;

  if (num==0)
  {
	  st[0]=48;
	  st[1]=46;
	  for ( i=0; i<dec; i++)
		  st[2+i]=48;
	  st[2+dec]=0;
  }
  else
  {
	  if (num<0)
	  {
		neg=true;
		num=-num;
	  }

	  if (dec<1)
		dec=1;
	  if (dec>5)
		dec=5;

	  inum=(long)(num*pow(10,dec));

	  while (inum>0)
	  {
		buf[c]=48+(inum % 10);
		c++;
		inum=(inum-(inum % 10))/10;
	  }
	  if ((num<1) && (num>0))
	  {
		  buf[c]=48;
		  c++;
	  }
	  buf[c]=0;

	  if (neg)
	  {
		st[0]=45;
	  }

	  c2=neg;
	  for ( i=0; i<c; i++)
	  {
		st[c2]=buf[c-i-1];
		c2++;
		if ((c-(c2-neg))==dec)
		{
		  st[c2]=46;
		  c2++;
		}
	  }
	  st[c2]=0;
  }

  tft32_print(st,x,y,0);
}

/*
 * Set current font used to display characters
 * font = font array that represents letters and numbers
 * font_param = parameters that represents size, numbers of chars
 * and offset from first char
 */
void tft32_setFont(const uint8_t* font, const uint8_t* font_param)
{
	cfont.font = (uint8_t*)font;
	cfont.font_param = (uint8_t*)font_param;
	cfont.x_size = font_param[0];
	cfont.y_size = font_param[1];
	cfont.offset = font_param[2];
	cfont.numchars = font_param[3];
	cfont.Hrows = cfont.x_size/8;
	if (cfont.x_size % 8) cfont.Hrows++;
}

void tft32_drawBitmap_(int x, int y, int sx, int sy, uint16_t* data, int scale)
{
	uint16_t col;
	int tx, ty, tc, tsx, tsy;
	//byte r, g, b;

	if (scale==1)
	{
		if (ptft->orient==PORTRAIT)
		{
			tft32_setXY(x, y, x+sx-1, y+sy-1);
			for (tc=0; tc<(sx*sy); tc++)
			{
				col=data[tc];
				tft32_LCD_Write_DATA(col);
			}
		}
		else
		{
			for (ty=0; ty<sy; ty++)
			{
				tft32_setXY(x, y+ty, x+sx-1, y+ty);
				for (tx=sx; tx>=0; tx--)
				{
					col=data[(ty*sx)+tx];
					tft32_LCD_Write_DATA(col);
				}
			}
		}
	}
	else
	{
		if (ptft->orient==PORTRAIT)
		{
			for (ty=0; ty<sy; ty++)
			{
				tft32_setXY(x, y+(ty*scale), x+((sx*scale)-1), y+(ty*scale)+scale);
				for (tsy=0; tsy<scale; tsy++)
					for (tx=0; tx<sx; tx++)
					{
						col=data[(ty*sx)+tx];
						for (tsx=0; tsx<scale; tsx++)
							tft32_LCD_Write_DATA(col);
					}
			}
		}
		else
		{
			for (ty=0; ty<sy; ty++)
			{
				for (tsy=0; tsy<scale; tsy++)
				{
					tft32_setXY(x, y+(ty*scale)+tsy, x+((sx*scale)-1), y+(ty*scale)+tsy);
					for (tx=sx; tx>=0; tx--)
					{
						col=data[(ty*sx)+tx];
						for (tsx=0; tsx<scale; tsx++)
							tft32_LCD_Write_DATA(col);
					}
				}
			}
		}
	}
}

void tft32_drawBitmap(int x, int y, int sx, int sy, uint16_t* data, int deg, int rox, int roy)
{
	unsigned int col;
	int tx, ty, newx, newy;
	//byte r, g, b;
	double radian;
	radian=deg*0.0175;

	if (deg==0)
		tft32_drawBitmap_(x, y, sx, sy, data,1);
	else
	{
		for (ty=0; ty<sy; ty++)
		{
			for (tx=0; tx<sx; tx++)
			{
				col=data[(ty*sx)+tx];

				newx=x+rox+(((tx-rox)*cos(radian))-((ty-roy)*sin(radian)));
				newy=y+roy+(((ty-roy)*cos(radian))+((tx-rox)*sin(radian)));

				tft32_setXY(newx, newy, newx, newy);
				tft32_LCD_Write_DATA(col);
			}
		}
	}
}

int tft32_getDisplayXSize(void)
{
	if (ptft->orient==PORTRAIT)
		return ptft->disp_x_size+1;
	else
		return ptft->disp_y_size+1;
}

int tft32_getDisplayYSize(void)
{
	if (ptft->orient==PORTRAIT)
		return ptft->disp_y_size+1;
	else
		return ptft->disp_x_size+1;
}

uint8_t* tft32_getFont(void)
{
	return cfont.font;
}

uint8_t* tft32_getFont_param(void)
{
	return cfont.font_param;
}


uint8_t tft32_getFontXsize(void)
{
	return cfont.x_size;
}

uint8_t tft32_getFontYsize(void)
{
	return cfont.y_size;
}

uint16_t tft32_getColor(void)
{
	return ptft->fgcolor;
}

uint16_t tft32_getBackColor(void)
{
	return ptft->bgcolor;
}
