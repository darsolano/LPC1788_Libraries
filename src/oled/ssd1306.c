/*
 * File:   ssd1306.c
 * Must include ssd1306.h
 * Author: dsolano
 * I2C Driven SSD1306 or SH1106 SaintSmart OLED LCD 128x64
 * Created on July 5, 2014, 1:06 AM
 * NXP LPCXpresso 1769
 */
#include <oled/ssd1306.h>
#include <oled/font5x7.h>
#include <BSP_Waveshare/bsp_waveshare.h>
#include <string.h>

#if USE_SPI
#include <define_pins.h>
#endif

#if USE_I2C
typedef struct{
	uint8_t i2c_address_7bits;
	uint8_t i2c_address_8bits;
	int 	i2c_rate;
	I2C_ID_T i2c_bus_id;
	I2C_EVENTHANDLER_T i2c_event_hdlr;
}SSD1306_t;
SSD1306_t I2Cx_ssd1306;
PRIVATE uint8_t buff[12];
#endif

#if USE_SPI
DEFINE_PIN(OLED_CS,1,21)	// SS Pin on SSP1
DEFINE_PIN(OLED_D_C,1,30)	// D = Active HIGH; C = Active LOW
DEFINE_PIN(OLED_RST,1,31)	// ON or OFF
#endif

Status GLCDstatus;
PRIVATE uint8_t shadowFB[SHADOW_FB_SIZE];
PRIVATE uint8_t const font_mask[] =
{ 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01,0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };

PRIVATE const uint8_t GLCD_Setup_Codes[] = {
		    GLCD_DISP_OFF,
		    GLCD_SET_RATIO_OSC,
			0x80,
			GLCD_MULTIPLEX,
			0x3F,
			GLCD_SET_OFFSET,
			0x00,
			GLCD_SET_LINE,
			GLCD_CHARGE_PUMP,
			0x14,
			GLCD_MEM_ADDRESSING,
			0x00,
			GLCD_SET_SEG_REMAP0 | 0x1,
			GLCD_SET_SCAN_NOR,
			GLCD_SET_PADS,
			0x12,
			GLCD_SET_CONTRAST,
			0xFF,
			GLCD_SET_CHARGE,
			0xF1,
			GLCD_SET_VCOM,
			0x40,
			GLCD_EON_OFF,
			GLCD_DISP_NOR,
			GLCD_DISP_ON

};


PRIVATE void GLCDResetSSD1306(void);
PRIVATE void GLCDWriteCommand(uint8_t data);
PRIVATE void GLCDWriteData(uint8_t data);
PRIVATE void GLCD_vLine(uint8_t x0, uint8_t y0, uint8_t y1, oled_color_t color);
PRIVATE void GLCD_hLine(uint8_t x0, uint8_t y0, uint8_t x1, oled_color_t color);
PRIVATE void GLCD_WriteDataLen(unsigned char data, unsigned int len);

void GLCD_UpdateScreen(void) {
	uint8_t i;

	for (i = 0xB0; i < 0xB8; i++)
	{            // Go through all 8 pages
		mSetAddress(i, 0x00, 0x10);

		/* Write multi data */
#if USE_I2C
		while (Chip_I2C_MasterSend(SSD1306_I2C_BUS, SSD1306_I2C_ADDR, &shadowFB[OLED_DISPLAY_WIDTH * i], OLED_DISPLAY_WIDTH) < OLED_DISPLAY_WIDTH);
#endif
#if USE_SPI
		GLCD_WriteDataLen(shadowFB[OLED_DISPLAY_WIDTH * i], OLED_DISPLAY_WIDTH);
#endif
	}
}

#if USE_SPI
uint8_t GLCD_spiIO(uint8_t data)
{
	while (!(OLEDSSPx->SR & SSP_STAT_TFE));
	OLEDSSPx->DR = data;		// send a byte
	while (!(OLEDSSPx->SR & SSP_STAT_RNE));
	return OLEDSSPx->DR;		// Receive a byte
}
#endif

PRIVATE void GLCDWriteCommand(uint8_t cmd)
{
#if USE_I2C
	buff[0] = 0x00;	// write Co & D/C bits
	buff[1] = cmd;	// Command

	while (Chip_I2C_MasterSend(SSD1306_I2C_BUS, SSD1306_I2C_ADDR, buff, 2) < 2);
#endif

#if USE_SPI
	OLED_D_C_LOW();
	OLED_CS_SELECTED();
	GLCD_spiIO(cmd);
	OLED_CS_DESELECTED();
#endif
}

PRIVATE void GLCDWriteData(uint8_t data)
{
#if USE_I2C
	buff[0] = 0x40;	// write Co & D/C bits
	buff[1] = data;	// data

	while (Chip_I2C_MasterSend(SSD1306_I2C_BUS, SSD1306_I2C_ADDR, buff, 2) < 2);
#endif

#if USE_SPI
	OLED_D_C_HIGH();
	OLED_CS_SELECTED();
	GLCD_spiIO(data);
	OLED_CS_DESELECTED();
#endif
}

/** Set up everything so that the display is ready to start having text
 * rendered on it. By default, this will not turn on the display; use
 * displayOn() to do that. */
int GLCD_SSD1306Init(FunctionalState I2CINIT)
{
#if USE_I2C
	uint8_t dummy = 0;
	I2Cx_ssd1306.i2c_address_7bits = 0x3c;
	I2Cx_ssd1306.i2c_address_8bits = 0x78;
	I2Cx_ssd1306.i2c_bus_id = I2C1;
	I2Cx_ssd1306.i2c_rate = 400000;
	I2Cx_ssd1306.i2c_event_hdlr = Chip_I2C_EventHandlerPolling;

	if (I2CINIT){
		wsBoard_I2C_Init(SSD1306_I2C_BUS);
		Chip_I2C_Init(SSD1306_I2C_BUS);
		Chip_I2C_SetClockRate(SSD1306_I2C_BUS, 400000);
		Chip_I2C_SetMasterEventHandler(SSD1306_I2C_BUS, Chip_I2C_EventHandlerPolling);
	}

	// to check if device is present on i2c bus
	if (!Chip_I2C_MasterSend(SSD1306_I2C_BUS, SSD1306_I2C_ADDR, &dummy, 1))
		return 0;
#endif
#if USE_SPI
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

	GLCDResetSSD1306();
	GLCDisplayOn();
	return 1;
}

/** Turn display on. This must be called in order to have anything appear
 * on the screen. */
void GLCDisplayOn(void)
{
	GLCDWriteCommand(0x8D);
	GLCDWriteCommand(0x14);
	GLCDWriteCommand( GLCD_DISP_ON); // display on
}

/** Turn display off. This will cause the SSD1306 controller to enter a
 * low power state. */
void GLCDisplayOff(void)
{
	GLCDWriteCommand(0x8D);
	GLCDWriteCommand(0x10);
	GLCDWriteCommand( GLCD_DISP_OFF); // display off
}

/******************************************************************************
 *
 * Description:
 *    Write len number of same data to the display
 *
 * Params:
 *   [in] data - data (color) to write to the display
 *   [in] len  - number of bytes to write
 *
 *****************************************************************************/
PRIVATE void GLCD_WriteDataLen(unsigned char data, unsigned int len)
{
#if USE_I2C
	int i;
	uint8_t buf[140];

	buf[0] = 0x40; // write Co & D/C bits

	for (i = 1; i < len + 1; i++)
	{
		buf[i] = data;
	}

	while (Chip_I2C_MasterSend(SSD1306_I2C_BUS, SSD1306_I2C_ADDR, buf, len) < len);
#endif
#if USE_SPI
	OLED_D_C_HIGH();
	OLED_CS_SELECTED();

	while (len--)
		GLCD_spiIO(data);

	OLED_CS_DESELECTED();
#endif

}

/** Reset and initialise the SSD1306 display controller. This mostly follows
 * Figure 2 ("Software Initialization Flow Chart") on page 64 of the
 * SSD1306 datasheet.
 * This does not turn the display on (displayOn() does that). This is so
 * that display GDDRAM can be cleared. If the display is turned on
 * immediately, undefined junk will appear on the display.
 */
PRIVATE void GLCDResetSSD1306(void)
{
	/*
	 * Recommended Initial code according to manufacturer
	 */
	int i;
	int t = sizeof(GLCD_Setup_Codes)/sizeof(uint8_t);

	for (i=0;i<t;i++){
		GLCDWriteCommand(GLCD_Setup_Codes[i]);
	}

	GLCD_ClearScreen(OLED_COLOR_BLACK);
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
void GLCD_ClearScreen(oled_color_t color)
{
	uint8_t i;
	uint8_t c = 0;

	if (color == OLED_COLOR_WHITE)
		c = 0xff;

	for (i = 0xB0; i < 0xB8; i++)
	{            // Go through all 8 pages
		mSetAddress(i, 0x00, 0x10);
		GLCD_WriteDataLen(c, 132);
	}
	memset(shadowFB, c, SHADOW_FB_SIZE);
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
PRIVATE void GLCD_hLine(uint8_t x0, uint8_t y0, uint8_t x1, oled_color_t color)
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
		GLCD_putPixel(x0, y0, color);
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
PRIVATE void GLCD_vLine(uint8_t x0, uint8_t y0, uint8_t y1, oled_color_t color)
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
		GLCD_putPixel(x0, y0, color);
		y0++;
	}
	return;
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
void GLCD_Line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,
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
		GLCD_vLine(x0, y0, y1, color);
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
		GLCD_hLine(x0, y0, x1, color);
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

			GLCD_putPixel(x0, y0, color);
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
		GLCD_putPixel(x0, y0, color);
	}
	else
	{
		di = dx_x2 - dy;
		while (y0 != y1)
		{
			GLCD_putPixel(x0, y0, color);
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
		GLCD_putPixel(x0, y0, color);
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
void GLCD_circle(uint8_t x0, uint8_t y0, uint8_t r, oled_color_t color)
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
		GLCD_putPixel(draw_x0, draw_y0, color); /* 90 degree */
	}

	draw_x2 = draw_x3 = x0;
	draw_y2 = draw_y3 = y0 - r;
	if (draw_y2 >= 0)
	{
		GLCD_putPixel(draw_x2, draw_y2, color); /* 270 degree */
	}

	draw_x4 = draw_x6 = x0 + r;
	draw_y4 = draw_y6 = y0;
	if (draw_x4 < OLED_DISPLAY_WIDTH)
	{
		GLCD_putPixel(draw_x4, draw_y4, color); /* 0 degree */
	}

	draw_x5 = draw_x7 = x0 - r;
	draw_y5 = draw_y7 = y0;
	if (draw_x5 >= 0)
	{
		GLCD_putPixel(draw_x5, draw_y5, color); /* 180 degree */
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
			GLCD_putPixel(draw_x0, draw_y0, color);
		}

		if ((draw_x1 >= 0) && (draw_y1 >= 0))
		{
			GLCD_putPixel(draw_x1, draw_y1, color);
		}

		if ((draw_x2 <= OLED_DISPLAY_WIDTH) && (draw_y2 <= OLED_DISPLAY_HEIGHT))
		{
			GLCD_putPixel(draw_x2, draw_y2, color);
		}

		if ((draw_x3 >= 0) && (draw_y3 <= OLED_DISPLAY_HEIGHT))
		{
			GLCD_putPixel(draw_x3, draw_y3, color);
		}

		if ((draw_x4 <= /*OLED_DISPLAY_HEIGHT*/OLED_DISPLAY_WIDTH)
				&& (draw_y4 >= 0))
		{
			GLCD_putPixel(draw_x4, draw_y4, color);
		}

		if ((draw_x5 >= 0) && (draw_y5 >= 0))
		{
			GLCD_putPixel(draw_x5, draw_y5, color);
		}
		if ((draw_x6 <= OLED_DISPLAY_WIDTH) && (draw_y6 <= OLED_DISPLAY_HEIGHT))
		{
			GLCD_putPixel(draw_x6, draw_y6, color);
		}
		if ((draw_x7 >= 0) && (draw_y7 <= OLED_DISPLAY_HEIGHT))
		{
			GLCD_putPixel(draw_x7, draw_y7, color);
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
void GLCD_rect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,
		oled_color_t color)
{
	GLCD_hLine(x0, y0, x1, color);
	GLCD_hLine(x0, y1, x1, color);
	GLCD_vLine(x0, y0, y1, color);
	GLCD_vLine(x1, y0, y1, color);
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
void GLCD_fillRect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,
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
		GLCD_hLine(x0, y0, x1, color);
		return;
	}

	if (x0 == x1)
	{
		GLCD_vLine(x0, y0, y1, color);
		return;
	}

	while (y0 <= y1)
	{
		GLCD_hLine(x0, y0, x1, color);
		y0++;
	}
	return;
}

uint8_t GLCD_putChar(uint8_t x, uint8_t y, uint8_t ch, oled_color_t fb,
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
				GLCD_putPixel(x, y, color);
				x++;
			}
		}
		y++;
		x -= pfont->width;
	}
	return (1);
}

uint8_t GLCD_putChar_5x7(uint8_t x, uint8_t y, uint8_t ch, oled_color_t fb, oled_color_t bg)
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
            GLCD_putPixel(x, y, color);
            x++;
        }
        y++;
        x -= 6;
    }
    return( 1 );
}

void GLCD_putString(uint8_t x, uint8_t y, uint8_t *pStr, oled_color_t fb,
		oled_color_t bg, FONTCFG_Typedef* pfont)
{
	while (1)
	{
		if ((*pStr) == '\0')
		{
			break;
		}
		if (pfont->font != 0)	// Weather 5x7 or other font is used, just equals font pointer to 0
		{
			if (GLCD_putChar(x, y, *pStr++, fb, bg , pfont) == 0) break;
			x += pfont->width;
		}
		else
		{
			if (GLCD_putChar_5x7(x, y, *pStr++, fb, bg) == 0)  break;
			x += 6;
		}
	}
	return;
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
void GLCD_putPixel(uint8_t x, uint8_t y, oled_color_t color)
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

	mSetAddress(page, lAddr, hAddr)
	// Set the address (sets the page,
	// lower and higher column address pointers)

	shadowPos = (page - 0xB0) * OLED_DISPLAY_WIDTH + x;

	if (color > 0)
		shadowFB[shadowPos] |= mask;
	else
		shadowFB[shadowPos] &= ~mask;

	GLCDWriteData(shadowFB[shadowPos]);
}



Status GLCD_FontSetup( const uint8_t* fontarray, FONTCFG_Typedef* pfont, const uint8_t* font_param)
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
void GLCD_PutDec99(uint8_t decnum , uint8_t xpos, uint8_t ypos, oled_color_t char_color , oled_color_t bg_color, FONTCFG_Typedef *font)
{
	uint8_t buf[3];
	uint8_t c = 0;
	uint8_t d0,d1 = 0;

	if (decnum > 99) return;
	d1 = (decnum / 10) % 10;
	d0 =  decnum % 10;

	buf[c++] = d1 +'0';
	buf[c++] = d0 + '0';
	buf[c] = 0;	// End Of string
	GLCD_putString(xpos,ypos,buf,char_color,bg_color,font);
}

/*
 * @brief		Puts a decimal number to OLED 96x64 until 255
 * @param[in]	decnum	Decimal number (8-bit long)
 * @return		None
 */
void GLCD_PutDec(uint8_t decnum , uint8_t xpos, uint8_t ypos, oled_color_t char_color , oled_color_t bg_color, FONTCFG_Typedef *font)
{
	uint8_t buf[4];
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
	GLCD_putString(xpos,ypos,buf,char_color,bg_color,font);
}


/*********************************************************************//**
 * @brief		Puts a decimal number to OLED
 * @param[in]	decnum	Decimal number (8-bit long)
 * @return		None
 **********************************************************************/
void GLCD_PutDec16(uint16_t decnum , uint8_t xpos, uint8_t ypos, oled_color_t char_color , oled_color_t bg_color, FONTCFG_Typedef *font)
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

	GLCD_putString(xpos,ypos,buf,char_color,bg_color,font);
}

/*********************************************************************//**
 * @brief		Puts a decimal number to OLED
 * @param[in]	decnum	Decimal number (8-bit long)
 * @return		None
 **********************************************************************/
void GLCD_PutDec32(uint32_t decnum , uint8_t xpos, uint8_t ypos, oled_color_t char_color , oled_color_t bg_color, FONTCFG_Typedef *font)
{
	uint8_t buff[11] = {0};
	uint8_t dec[11] = {0};

	dec[0] = (decnum % 10);
	dec[1] = (decnum / 10) % 10;
	dec[2] = (decnum / 100) % 10;
	dec[3] = (decnum / 1000) % 10;
	dec[4] = (decnum / 10000) % 10;
	dec[5] = (decnum / 100000) % 10;
	dec[6] = (decnum / 1000000) % 10;
	dec[7] = (decnum / 10000000) % 10;
	dec[8] = (decnum / 100000000) % 10;
	dec[9] = (decnum / 1000000000) % 10;

	buff[0] = ('0' + dec[9]);
	buff[1] = ('0' + dec[8]);
	buff[2] = ('0' + dec[7]);
	buff[3] = ('0' + dec[6]);
	buff[4] = ('0' + dec[5]);
	buff[5] = ('0' + dec[4]);
	buff[6] = ('0' + dec[3]);
	buff[7] = ('0' + dec[2]);
	buff[8] = ('0' + dec[1]);
	buff[9] = ('0' + dec[0]);
	buff[10] = 0;	// End Of string

	GLCD_putString(xpos,ypos,buff,char_color,bg_color,font);
}

/*********************************************************************//**
 * @brief		Puts a hex number to OLED display without the 0x chars
 * @param[in]	hexnum	Hex number (8-bit long)
 * @return		None
 **********************************************************************/
void GLCD_PutHex_(uint8_t hexnum , uint8_t xpos, uint8_t ypos, oled_color_t char_color , oled_color_t bg_color, FONTCFG_Typedef *font)
{
	uint8_t nibble, i, idx = 0;
	uint8_t buff[3] = {0};

	i = 1;

	do
	{
		nibble = (hexnum >> (4 * i)) & 0x0F;
		buff[idx++] = ((nibble > 9) ? ('A' + nibble - 10) : ('0' + nibble));
	} while (i--);

	buff[idx] = 0;

	GLCD_putString(xpos,ypos,buff,char_color,bg_color,font);
}


/*********************************************************************//**
 * @brief		Puts a hex number to OLED
 * @param[in]	hexnum	Hex number (8-bit long)
 * @return		None
 **********************************************************************/
void GLCD_PutHex(uint8_t hexnum , uint8_t xpos, uint8_t ypos, oled_color_t char_color , oled_color_t bg_color, FONTCFG_Typedef *font)
{
	uint8_t nibble, i, idx = 0;
	uint8_t buff[5] = {0};

	buff[idx++] = '0';
	buff[idx++] = 'x';

	i = 1;

	do
	{
		nibble = (hexnum >> (4 * i)) & 0x0F;
		buff[idx++] = ((nibble > 9) ? ('A' + nibble - 10) : ('0' + nibble));
	} while (i--);

	buff[idx] = 0;

	GLCD_putString(xpos,ypos,buff,char_color,bg_color,font);
}

/*********************************************************************//**
 * @brief		Puts a hex number to OLED
 * @param[in]	hexnum	Hex number (16-bit long)
 * @return		None
 **********************************************************************/
void GLCD_PutHex16(uint16_t hexnum , uint8_t xpos, uint8_t ypos, oled_color_t char_color , oled_color_t bg_color, FONTCFG_Typedef *font)
{
	uint8_t nibble, i, idx = 0;
	uint8_t buff[7] = {0};

	buff[idx++] = '0';
	buff[idx++] = 'x';

	i = 3;
	do
	{
		nibble = (hexnum >> (4 * i)) & 0x0F;
		buff[idx++] = ((nibble > 9) ? ('A' + nibble - 10) : ('0' + nibble));
	} while (i--);

	buff[idx] = 0;

	GLCD_putString(xpos,ypos,buff,char_color,bg_color,font);
}

/*********************************************************************//**
 * @brief		Puts a hex number to LCD-I2C
 * @param[in]	hexnum	Hex number (32-bit long)
 * @return		None
 **********************************************************************/
void GLCD_PutHex32(uint32_t hexnum , uint8_t xpos, uint8_t ypos, oled_color_t char_color , oled_color_t bg_color, FONTCFG_Typedef *font)
{
	uint8_t nibble, i, idx = 0;
	uint8_t buff[11] = {0};

	buff[idx++] = '0';
	buff[idx++] = 'x';

	i = 7;

	do
	{
		nibble = (hexnum >> (4 * i)) & 0x0F;
		buff[idx++] = ((nibble > 9) ? ('A' + nibble - 10) : ('0' + nibble));
	} while (i--);

	buff[idx] = 0;

	GLCD_putString(xpos,ypos,buff,char_color,bg_color,font);
}
