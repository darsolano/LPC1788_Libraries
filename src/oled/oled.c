/*****************************************************************************
 *   light.c:  Driver for the ISL29003 Light-to-Digital Output Sensor
 *
 *   Copyright(C) 2009, Embedded Artists AB
 *   All rights reserved.
 *
 ******************************************************************************/

/*
 * NOTE: I2C or SPI must have been initialized before calling any functions in
 * this file.
 *
 *
 */

/******************************************************************************
 * Includes
 *****************************************************************************/

#include <string.h>
#include <oled/oled.h>
#include <oled/font5x7.h>
#include <define_pins.h>


/******************************************************************************
 * Defines and typedefs
 *****************************************************************************/

#ifdef OLED_USE_I2C
#else
#define OLEDSSPx	LPC_SSP0
DEFINE_PIN(OLED_CS,1,21)	// SS Pin on SSP1
DEFINE_PIN(OLED_D_C,1,30)	// D = Active HIGH; C = Active LOW
DEFINE_PIN(OLED_RST,1,31)	// ON or OFF
#endif



/*
 * The display controller can handle a resolution of 132x64. The OLED
 * on the base board is 96x64.
 */
#define X_OFFSET 2

#define SHADOW_FB_SIZE (OLED_DISPLAY_WIDTH*OLED_DISPLAY_HEIGHT >> 3)

#define setAddress(page,lowerAddr,higherAddr)\
    writeCommand(page);\
    writeCommand(lowerAddr);\
    writeCommand(higherAddr);

/******************************************************************************
 * External global variables
 *****************************************************************************/

/******************************************************************************
 * Local variables
 *****************************************************************************/

/*
 * The SSD1305 doesn't support reading from the display memory when using
 * serial mode (only parallel mode). Since it isn't possible to write only
 * one pixel to the display (a minimum of one column, 8 pixels, is always
 * Written) a shadow framebuffer is needed to keep track of the display
 * data.
 */
static uint8_t shadowFB[SHADOW_FB_SIZE];

static uint8_t const font_mask[8] =
{ 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };

/******************************************************************************
 * Local Functions
 *****************************************************************************/
#ifdef OLED_USE_I2C
static int I2CWrite(uint8_t addr, uint8_t* buf, uint32_t len)
{
	I2C_XFER_T txsetup;

	txsetup.slaveAddr = addr;
	txsetup.txBuff = buf;
	txsetup.txSz = len;
	txsetup.rxBuff = NULL;
	txsetup.rxSz = 0;

	if (Chip_I2C_MasterTransfer(OLED_I2C_ID, &txsetup) == I2C_STATUS_DONE)
	{
		return (0);
	}
	else
	{
		return (-1);
	}
}
#else
uint8_t oled_spiIO(uint8_t data)
{
	while (!(OLEDSSPx->SR & SSP_STAT_TFE));
	OLEDSSPx->DR = data;		// send a byte
	while (!(OLEDSSPx->SR & SSP_STAT_RNE));
	return OLEDSSPx->DR;		// Receive a byte
}

#endif

/******************************************************************************
 *
 * Description:
 *    Write a command to the display
 *
 * Params:
 *   [in] data - command to write to the display
 *
 *****************************************************************************/
static void writeCommand(uint8_t data)
{

#ifdef OLED_USE_I2C
	uint8_t buf[2];

	buf[0] = 0x00; // write Co & D/C bits
	buf[1] = data;// data

	//I2CWrite(OLED_I2C_ADDR, buf, 2);
	Chip_I2C_MasterSend(OLED_I2C_ID, OLED_I2C_ADDR, buf, 2);

#else
	OLED_D_C_LOW();
	OLED_CS_SELECTED();
	oled_spiIO(data);
	OLED_CS_DESELECTED();
#endif
}

/******************************************************************************
 *
 * Description:
 *    Write data to the display
 *
 * Params:
 *   [in] data - data (color) to write to the display
 *
 *****************************************************************************/
static void writeData(uint8_t data)
{
#ifdef OLED_USE_I2C
	uint8_t buf[2];

	buf[0] = 0x40; // write Co & D/C bits
	buf[1] = data;// data

	I2CWrite(OLED_I2C_ADDR, buf, 2);

#else
	OLED_D_C_HIGH();
	OLED_CS_SELECTED();
	oled_spiIO(data);
	OLED_CS_DESELECTED();
#endif
}

/******************************************************************************
 *
 * Description:
 *    Write len number of data to the display
 *
 * Params:
 *   [in] data - data (color) to write to the display
 *   [in] len  - number of bytes to write
 *
 *****************************************************************************/
static void writeDataLen(unsigned char data, unsigned int len)
{
	// TODO: optimize (at least from a RAM point of view)

#ifdef OLED_USE_I2C
	int i;
	uint8_t buf[140];

	buf[0] = 0x40; // write Co & D/C bits

	for (i = 1; i < len+1; i++)
	{
		buf[i] = data;
	}

	I2CWrite(OLED_I2C_ADDR, buf, len+1);

#else
	OLED_D_C_HIGH();
	OLED_CS_SELECTED();

	while (len--)
		oled_spiIO(data);

	OLED_CS_DESELECTED();
#endif
}

/******************************************************************************
 *
 * Description:
 *    Run display init sequence
 *
 *****************************************************************************/
static void runInitSequence(void)
{
	/*
	 * Recommended Initial code according to manufacturer
	 */
	writeCommand(OLED_DISP_OFF);
	writeCommand(OLED_SET_RATIO_OSC);
	writeCommand(0x80);
	writeCommand(OLED_MULTIPLEX);
	writeCommand(0x3F);
	writeCommand(OLED_SET_OFFSET);
	writeCommand(0x00);
	writeCommand(OLED_SET_LINE);
	writeCommand(OLED_CHARGE_PUMP);
	writeCommand(0x14);
	writeCommand(OLED_MEM_ADDRESSING);
	writeCommand(0x00);
	writeCommand(OLED_SET_SEG_REMAP0 | 0x1);
	writeCommand(OLED_SET_SCAN_NOR);
	writeCommand(OLED_SET_PADS);
	writeCommand(0x12);
	writeCommand(OLED_SET_CONTRAST);
	writeCommand(0xCF);
	writeCommand(OLED_SET_CHARGE);
	writeCommand(0xF1);
	writeCommand(OLED_SET_VCOM);
	writeCommand(0x40);
	writeCommand(OLED_EON_OFF);
	writeCommand(OLED_DISP_NOR);
	writeCommand(OLED_DISP_ON);

}

/******************************************************************************
 *
 * Description:
 *    Draw a horizontal line
 *
 * Params:
 *   [in] x0 - start x position
 *   [in] y0 - y position
 *   [in] x1 - end y position
 *   [in] color - color of the pixel
 *
 *****************************************************************************/
static void hLine(uint8_t x0, uint8_t y0, uint8_t x1, oled_color_t color)
{
	// TODO: optimize

	uint8_t bak;

	if (x0 > x1)
	{
		bak = x1;
		x1 = x0;
		x0 = bak;
	}

	while (x1 >= x0)
	{
		oled_putPixel(x0, y0, color);
		x0++;
	}
}

/******************************************************************************
 *
 * Description:
 *    Draw a vertical line
 *
 * Params:
 *   [in] x0 - x position
 *   [in] y0 - start y position
 *   [in] y1 - end y position
 *   [in] color - color of the pixel
 *
 *****************************************************************************/
static void vLine(uint8_t x0, uint8_t y0, uint8_t y1, oled_color_t color)
{
	uint8_t bak;

	if (y0 > y1)
	{
		bak = y1;
		y1 = y0;
		y0 = bak;
	}

	while (y1 >= y0)
	{
		oled_putPixel(x0, y0, color);
		y0++;
	}
	return;
}

/******************************************************************************
 * Public Functions
 *****************************************************************************/

/******************************************************************************
 *
 * Description:
 *    Initialize the OLED Display
 *
 *****************************************************************************/
void oled_init(void)
{
	int i;
#ifdef OLED_USE_I2C
	Chip_I2C_Init(OLED_I2C_ID);
	Chip_I2C_SetClockRate(OLED_I2C_ID, OLED_I2C_RATE);
	Chip_I2C_SetMasterEventHandler(OLED_I2C_ID, Chip_I2C_EventHandlerPolling);
//	Chip_GPIO_ClearValue(LPC_GPIO, 2, (1<<7)); // D/C#
//	Chip_GPIO_ClearValue(LPC_GPIO, 0, (1<<6));// CS#
#else
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

	OLED_CS_OUTPUT();
	OLED_D_C_OUTPUT();
	OLED_RST_OUTPUT();
	Chip_SSP_Init(OLEDSSPx);
	Chip_SSP_SetMaster(OLEDSSPx, TRUE);
	Chip_SSP_SetBitRate(OLEDSSPx, 1000000);
	Chip_SSP_Enable(OLEDSSPx);
	OLED_CS_DESELECTED();
	OLED_RST_ON();
#endif

	runInitSequence();

	memset(shadowFB, 0, SHADOW_FB_SIZE);

	/* small delay before turning on power */
	for (i = 0; i < 0xffff; i++)
		;

	/* power on */
	//OLED_PWR_ON();
	oled_clearScreen(OLED_COLOR_BLACK);
}

/******************************************************************************
 *
 * Description:
 *    Draw one pixel on the display
 *
 * Params:
 *   [in] x - x position
 *   [in] y - y position
 *   [in] color - color of the pixel
 *
 *****************************************************************************/
void oled_putPixel(uint8_t x, uint8_t y, oled_color_t color)
{
	uint8_t page;
	uint16_t add;
	uint8_t lAddr;
	uint8_t hAddr;
	uint8_t mask;
	uint32_t shadowPos = 0;

	if (x > OLED_DISPLAY_WIDTH)
	{
		return;
	}
	if (y > OLED_DISPLAY_HEIGHT)
	{
		return;
	}

	/* page address */
	if (y < 8)
		page = 0xB0;
	else if (y < 16)
		page = 0xB1;
	else if (y < 24)
		page = 0xB2;
	else if (y < 32)
		page = 0xB3;
	else if (y < 40)
		page = 0xB4;
	else if (y < 48)
		page = 0xB5;
	else if (y < 56)
		page = 0xB6;
	else
		page = 0xB7;

	add = x + X_OFFSET;
	lAddr = 0x0F & add;             // Low address
	hAddr = 0x10 | (add >> 4);      // High address

	// Calculate mask from rows basically do a y%8 and remainder is bit position
	add = y >> 3;                     // Divide by 8
	add <<= 3;                      // Multiply by 8
	add = y - add;                  // Calculate bit position
	mask = 1 << add;                // Left shift 1 by bit position

	setAddress(page, lAddr, hAddr); // Set the address (sets the page,
	// lower and higher column address pointers)

	shadowPos = (page - 0xB0) * OLED_DISPLAY_WIDTH + x;

	if (color > 0)
		shadowFB[shadowPos] |= mask;
	else
		shadowFB[shadowPos] &= ~mask;

	writeData(shadowFB[shadowPos]);
}

/******************************************************************************
 *
 * Description:
 *    Draw a line on the display starting at x0,y0 and ending at x1,y1
 *
 * Params:
 *   [in] x0 - start x position
 *   [in] y0 - start y position
 *   [in] x1 - end x position
 *   [in] y1 - end y position
 *   [in] color - color of the line
 *
 *****************************************************************************/
void oled_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,
		oled_color_t color)
{
	int16_t dx = 0, dy = 0;
	int8_t dx_sym = 0, dy_sym = 0;
	int16_t dx_x2 = 0, dy_x2 = 0;
	int16_t di = 0;

	dx = x1 - x0;
	dy = y1 - y0;

	if (dx == 0) /* vertical line */
	{
		vLine(x0, y0, y1, color);
		return;
	}

	if (dx > 0)
	{
		dx_sym = 1;
	}
	else
	{
		dx_sym = -1;
	}

	if (dy == 0) /* horizontal line */
	{
		hLine(x0, y0, x1, color);
		return;
	}

	if (dy > 0)
	{
		dy_sym = 1;
	}
	else
	{
		dy_sym = -1;
	}

	dx = dx_sym * dx;
	dy = dy_sym * dy;

	dx_x2 = dx * 2;
	dy_x2 = dy * 2;

	if (dx >= dy)
	{
		di = dy_x2 - dx;
		while (x0 != x1)
		{

			oled_putPixel(x0, y0, color);
			x0 += dx_sym;
			if (di < 0)
			{
				di += dy_x2;
			}
			else
			{
				di += dy_x2 - dx_x2;
				y0 += dy_sym;
			}
		}
		oled_putPixel(x0, y0, color);
	}
	else
	{
		di = dx_x2 - dy;
		while (y0 != y1)
		{
			oled_putPixel(x0, y0, color);
			y0 += dy_sym;
			if (di < 0)
			{
				di += dx_x2;
			}
			else
			{
				di += dx_x2 - dy_x2;
				x0 += dx_sym;
			}
		}
		oled_putPixel(x0, y0, color);
	}
	return;
}

/******************************************************************************
 *
 * Description:
 *    Draw a circle on the display starting at x0,y0 with radius r
 *
 * Params:
 *   [in] x0 - start x position
 *   [in] y0 - start y position
 *   [in] r - radius
 *   [in] color - color of the circle
 *
 *****************************************************************************/
void oled_circle(uint8_t x0, uint8_t y0, uint8_t r, oled_color_t color)
{
	int16_t draw_x0, draw_y0;
	int16_t draw_x1, draw_y1;
	int16_t draw_x2, draw_y2;
	int16_t draw_x3, draw_y3;
	int16_t draw_x4, draw_y4;
	int16_t draw_x5, draw_y5;
	int16_t draw_x6, draw_y6;
	int16_t draw_x7, draw_y7;
	int16_t xx, yy;
	int16_t di;

	if (r == 0) /* no radius */
	{
		return;
	}

	draw_x0 = draw_x1 = x0;
	draw_y0 = draw_y1 = y0 + r;
	if (draw_y0 < OLED_DISPLAY_HEIGHT)
	{
		oled_putPixel(draw_x0, draw_y0, color); /* 90 degree */
	}

	draw_x2 = draw_x3 = x0;
	draw_y2 = draw_y3 = y0 - r;
	if (draw_y2 >= 0)
	{
		oled_putPixel(draw_x2, draw_y2, color); /* 270 degree */
	}

	draw_x4 = draw_x6 = x0 + r;
	draw_y4 = draw_y6 = y0;
	if (draw_x4 < OLED_DISPLAY_WIDTH)
	{
		oled_putPixel(draw_x4, draw_y4, color); /* 0 degree */
	}

	draw_x5 = draw_x7 = x0 - r;
	draw_y5 = draw_y7 = y0;
	if (draw_x5 >= 0)
	{
		oled_putPixel(draw_x5, draw_y5, color); /* 180 degree */
	}

	if (r == 1)
	{
		return;
	}

	di = 3 - 2 * r;
	xx = 0;
	yy = r;
	while (xx < yy)
	{

		if (di < 0)
		{
			di += 4 * xx + 6;
		}
		else
		{
			di += 4 * (xx - yy) + 10;
			yy--;
			draw_y0--;
			draw_y1--;
			draw_y2++;
			draw_y3++;
			draw_x4--;
			draw_x5++;
			draw_x6--;
			draw_x7++;
		}
		xx++;
		draw_x0++;
		draw_x1--;
		draw_x2++;
		draw_x3--;
		draw_y4++;
		draw_y5++;
		draw_y6--;
		draw_y7--;

		if ((draw_x0 <= OLED_DISPLAY_WIDTH) && (draw_y0 >= 0))
		{
			oled_putPixel(draw_x0, draw_y0, color);
		}

		if ((draw_x1 >= 0) && (draw_y1 >= 0))
		{
			oled_putPixel(draw_x1, draw_y1, color);
		}

		if ((draw_x2 <= OLED_DISPLAY_WIDTH) && (draw_y2 <= OLED_DISPLAY_HEIGHT))
		{
			oled_putPixel(draw_x2, draw_y2, color);
		}

		if ((draw_x3 >= 0) && (draw_y3 <= OLED_DISPLAY_HEIGHT))
		{
			oled_putPixel(draw_x3, draw_y3, color);
		}

		if ((draw_x4 <= /*OLED_DISPLAY_HEIGHT*/OLED_DISPLAY_WIDTH)
				&& (draw_y4 >= 0))
		{
			oled_putPixel(draw_x4, draw_y4, color);
		}

		if ((draw_x5 >= 0) && (draw_y5 >= 0))
		{
			oled_putPixel(draw_x5, draw_y5, color);
		}
		if ((draw_x6 <= OLED_DISPLAY_WIDTH) && (draw_y6 <= OLED_DISPLAY_HEIGHT))
		{
			oled_putPixel(draw_x6, draw_y6, color);
		}
		if ((draw_x7 >= 0) && (draw_y7 <= OLED_DISPLAY_HEIGHT))
		{
			oled_putPixel(draw_x7, draw_y7, color);
		}
	}
	return;
}

/******************************************************************************
 *
 * Description:
 *    Draw a rectangle on the display starting at x0,y0 and ending at x1,y1
 *
 * Params:
 *   [in] x0 - start x position
 *   [in] y0 - start y position
 *   [in] x1 - end x position
 *   [in] y1 - end y position
 *   [in] color - color of the rectangle
 *
 *****************************************************************************/
void oled_rect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,
		oled_color_t color)
{
	hLine(x0, y0, x1, color);
	hLine(x0, y1, x1, color);
	vLine(x0, y0, y1, color);
	vLine(x1, y0, y1, color);
}

/******************************************************************************
 *
 * Description:
 *    Fill a rectangle on the display starting at x0,y0 and ending at x1,y1
 *
 * Params:
 *   [in] x0 - start x position
 *   [in] y0 - start y position
 *   [in] x1 - end x position
 *   [in] y1 - end y position
 *   [in] color - color of the rectangle
 *
 *****************************************************************************/
void oled_fillRect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,
		oled_color_t color)
{
	uint8_t i = 0;

	if (x0 > x1)
	{
		i = x0;
		x0 = x1;
		x1 = i;
	}

	if (y0 > y1)
	{
		i = y0;
		y0 = y1;
		y1 = i;
	}

	if (y0 == y1)
	{
		hLine(x0, y0, x1, color);
		return;
	}

	if (x0 == x1)
	{
		vLine(x0, y0, y1, color);
		return;
	}

	while (y0 <= y1)
	{
		hLine(x0, y0, x1, color);
		y0++;
	}
	return;
}

/******************************************************************************
 *
 * Description:
 *    Clear the entire screen
 *
 * Params:
 *   [in] color - color to fill the screen with
 *
 *****************************************************************************/
void oled_clearScreen(oled_color_t color)
{
	uint8_t i;
	uint8_t c = 0;

	if (color == OLED_COLOR_WHITE)
		c = 0xff;

	for (i = 0xB0; i < 0xB8; i++)
	{            // Go through all 8 pages
		setAddress(i, 0x00, 0x10);
		writeDataLen(c, 132);
	}

	memset(shadowFB, c, SHADOW_FB_SIZE);
}

uint8_t oled_putChar(uint8_t x, uint8_t y, uint8_t ch, oled_color_t fb,
		oled_color_t bg, FONTCFG_Typedef* pfont)
{
	uint8_t data = 0;
	uint8_t i = 0, j = 0, n = 0;
	//http://www.rinkydinkelectronics.com/index.php
	// To locate the exact byte to start with a char
	uint8_t* bmptr = (pfont->font + ((ch - pfont->firstchar) * pfont->height * pfont->fontHrows));

	oled_color_t color = OLED_COLOR_BLACK;

	if ((x >= (OLED_DISPLAY_WIDTH - pfont->width)) || (y >= (OLED_DISPLAY_HEIGHT - pfont->height)))
	{
		return 0;
	}

	for (i = 0; i < pfont->height; i++)
	{
		for(n=0;n<pfont->fontHrows;n++)
		{
			data = *(bmptr + i*pfont->fontHrows + n);
			for (j = 0; j < 8; j++)
			{
				if ((data & font_mask[j]) == 0)
				{
					color = bg;
				}
				else
				{
					color = fb;
				}
				oled_putPixel(x, y, color);
				x++;
			}
		}
		y++;
		x -= pfont->width;
	}
	return (1);
}


uint8_t oled_putChar_5x7(uint8_t x, uint8_t y, uint8_t ch, oled_color_t fb, oled_color_t bg)
{
    unsigned char data = 0;
    unsigned char i = 0, j = 0;

    oled_color_t color = OLED_COLOR_BLACK;

    if((x >= (OLED_DISPLAY_WIDTH - 8)) || (y >= (OLED_DISPLAY_HEIGHT - 8)) )
    {
        return 0;
    }

    if( (ch < 0x20) || (ch > 0x7f) )
    {
        ch = 0x20;      /* unknown character will be set to blank */
    }

    ch -= 0x20;
    for(i=0; i<8; i++)
    {
        data = font5x7[ch][i];
        for(j=0; j<6; j++)
        {
            if( (data&font_mask[j])==0 )
            {
                color = bg;
            }
            else
            {
                color = fb;
            }
            oled_putPixel(x, y, color);
            x++;
        }
        y++;
        x -= 6;
    }
    return( 1 );
}
void oled_putString(uint8_t x, uint8_t y, uint8_t *pStr, oled_color_t fb,
		oled_color_t bg, FONTCFG_Typedef *pfont, uint8_t align)
{
//	if (align == OLED_ALIGN_CENTER) x = (OLED_DISPLAY_WIDTH - strlen((char*)pStr)) / 2;
//	if (align == OLED_ALIGN_LEFT) x = x;
//	if (align == OLED_ALIGN_RIGHT) x += (OLED_DISPLAY_WIDTH - strlen((char*)pStr));

	while (1)
	{
		if ((*pStr) == '\0')
		{
			break;
		}

		if (pfont->font != 0)	// Weather 5x7 or other font is used, just equals font pointer to 0
		{
			if (oled_putChar(x, y, *pStr++, fb, bg , pfont) == 0) break;
			x += pfont->width;
		}
		else
		{
			if (oled_putChar_5x7(x, y, *pStr++, fb, bg) == 0)  break;
			x += 6;
		}
	}
	return;
}

Status oled_FontSetup( const uint8_t* fontarray, FONTCFG_Typedef* pfont, const uint8_t* font_param)
{
	if (fontarray == 0) return ERROR;
	else{
		pfont->font 		= (uint8_t*)fontarray;
		pfont->width 		= font_param[0];
		pfont->height 		= font_param[1];
		pfont->firstchar 	= font_param[2];
		pfont->charamount 	= font_param[3];
		pfont->fontHrows 	= pfont->width / 8;
		if (pfont->width  % 8) pfont->fontHrows++;
	}
	return SUCCESS;
}

/*********************************************************************//**
 * @brief		Puts a decimal number to OLED 96x64 until 99
 * @param[in]	decnum	Decimal number (8-bit long)
 * @return		None
 **********************************************************************/
void oled_PutDec99(uint8_t decnum , uint8_t xpos, uint8_t ypos, oled_color_t char_color , oled_color_t bg_color, FONTCFG_Typedef *font)
{
	uint8_t buf[2];
	uint8_t c = 0;
	uint8_t d0,d1 = 0;

	if (decnum > 99) return;
	d1 = (decnum / 10) % 10;
	d0 =  decnum % 10;

	buf[c++] = d1 +'0';
	buf[c++] = d0 + '0';
	buf[c] = 0;	// End Of string
	oled_putString(xpos,ypos,buf,char_color,bg_color,font,0);
}

/*
 * @brief		Puts a decimal number to OLED 96x64 until 255
 * @param[in]	decnum	Decimal number (8-bit long)
 * @return		None
 */
void oled_PutDec(uint8_t decnum , uint8_t xpos, uint8_t ypos, oled_color_t char_color , oled_color_t bg_color, FONTCFG_Typedef *font)
{
	uint8_t buf[3];
	uint8_t c = 0;
	uint8_t d0,d1,d2 = 0;

	if (decnum > 255) return;

	d2 = (decnum / 100) % 10;
	d1 = (decnum / 10) % 10;
	d0 =  decnum % 10;

	buf[c++] = d2 +'0';
	buf[c++] = d1 +'0';
	buf[c++] = d0 + '0';
	buf[c] = 0;	// End Of string
	oled_putString(xpos,ypos,buf,char_color,bg_color,font,0);
}


/*********************************************************************//**
 * @brief		Puts a decimal number to LCD-1602
 * @param[in]	decnum	Decimal number (8-bit long)
 * @return		None
 **********************************************************************/
void oled_PutDec16(uint16_t decnum , uint8_t xpos, uint8_t ypos, oled_color_t char_color , oled_color_t bg_color, FONTCFG_Typedef *font)
{
	uint8_t buf[5];
	uint8_t c = 0;
	uint8_t d0,d1,d2,d3,d4 = 0;

	if (decnum > 0xffff) return;

	d4 = (decnum / 10000) % 10;
	d3 = (decnum / 1000) % 10;
	d2 = (decnum / 100) % 10;
	d1 = (decnum / 10) % 10;
	d0 = decnum % 10;

	buf[c++] = d4 +'0';
	buf[c++] = d3 +'0';
	buf[c++] = d2 +'0';
	buf[c++] = d1 +'0';
	buf[c++] = d0 + '0';
	buf[c] = 0;	// End Of string

	oled_putString(xpos,ypos,buf,char_color,bg_color,font,0);
}
