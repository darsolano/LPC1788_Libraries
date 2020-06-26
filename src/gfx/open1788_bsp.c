/*
 * open1788_bsp.c
 *
 *  Created on: Jul 3, 2018
 *      Author: dsolano
 */

#include <chip.h>
#include <gfx/sdram_HY57V281620_X2.h>
#include <gfx/open1788_bsp.h>
#include <define_pins.h>
#include <iocon_17xx_40xx.h>
#include <string.h>
#include <math.h>
#include <gfx/TouchPanel/TouchPanel.h>

//STATIC const LCD_CONFIG_T lcd_tft420x272[] =
//{
//		C_GLCD_H_BACK_PORCH,
//		C_GLCD_H_FRONT_PORCH,
//		C_GLCD_H_PULSE,
//
//};

#define C_GLCD_CLK_PER_LINE     (C_GLCD_H_SIZE + C_GLCD_H_PULSE + C_GLCD_H_FRONT_PORCH + C_GLCD_H_BACK_PORCH)
#define C_GLCD_LINES_PER_FRAME  (C_GLCD_V_SIZE + C_GLCD_V_PULSE + C_GLCD_V_FRONT_PORCH + C_GLCD_V_BACK_PORCH)
#define C_GLCD_PIX_CLK          (C_GLCD_CLK_PER_LINE * C_GLCD_LINES_PER_FRAME * C_GLCD_REFRESH_FREQ)

#define PORTRAIT 0
#define LANDSCAPE 1

DEFINE_PIN(LCD_BLED, 2, 1)

LCD_DISPLAY_t lcd;
LCD_DISPLAY_t *plcd;
struct _current_font cfont;
/* Pin muxing configuration Waveshare Open1788*/
STATIC const PINMUX_GRP_T LCDpinmuxing[] = {
/* Touch SSP configuration pins */

		{ 1, 8, (IOCON_FUNC0 | IOCON_MODE_INACT) },		//Touch Chip Select Output GPIO
		{ 2, 15, (IOCON_FUNC0 | IOCON_MODE_INACT) },	//Touch PENIRQ Input
		{ 1, 18, (IOCON_FUNC5 | IOCON_MODE_INACT) },	//Touch MISO
		{ 0, 13, (IOCON_FUNC2 | IOCON_MODE_INACT) },	//Touch MOSI
		{ 1, 19, (IOCON_FUNC5 | IOCON_MODE_INACT) },	//Touch CLOCK

		/* LCD Data Pins*/
		{ 0, 4, (IOCON_FUNC7 | IOCON_FASTSLEW_EN) },	// LCD data pin D0 / R0
		{ 0, 5, (IOCON_FUNC7 | IOCON_FASTSLEW_EN) },	// LCD data pin D1 / R1
		{ 4, 28, (IOCON_FUNC7 | IOCON_FASTSLEW_EN) },	// LCD data pin D2 / R2
		{ 4, 29, (IOCON_FUNC7 | IOCON_FASTSLEW_EN) },	// LCD data pin D3 / R3
		{ 2, 6, (IOCON_FUNC7 | IOCON_FASTSLEW_EN) },	// LCD data pin D4 / R4
		{ 2, 7, (IOCON_FUNC7 | IOCON_FASTSLEW_EN) },	// LCD data pin D5 / R5
		{ 2, 8, (IOCON_FUNC7 | IOCON_FASTSLEW_EN) },	// LCD data pin D6 / R6
		{ 2, 9, (IOCON_FUNC7 | IOCON_FASTSLEW_EN) },	// LCD data pin D7 / R7
		{ 0, 6, (IOCON_FUNC7 | IOCON_FASTSLEW_EN) },	// LCD data pin D8 / G0
		{ 0, 7, (IOCON_FUNC7 | IOCON_FASTSLEW_EN) },	// LCD data pin D9 / G1
		{ 1, 20, (IOCON_FUNC7 | IOCON_FASTSLEW_EN) },	// LCD data pin D10 / G2
		{ 1, 21, (IOCON_FUNC7 | IOCON_FASTSLEW_EN) },	// LCD data pin D11 / G3
		{ 1, 22, (IOCON_FUNC7 | IOCON_FASTSLEW_EN) },	// LCD data pin D12 / G4
		{ 1, 23, (IOCON_FUNC7 | IOCON_FASTSLEW_EN) },	// LCD data pin D13 / G5
		{ 1, 24, (IOCON_FUNC7 | IOCON_FASTSLEW_EN) },	// LCD data pin D14 / G6
		{ 1, 25, (IOCON_FUNC7 | IOCON_FASTSLEW_EN) },	// LCD data pin D15 / G7
		{ 0, 8, (IOCON_FUNC7 | IOCON_FASTSLEW_EN) },	// LCD data pin D16 / B0
		{ 0, 9, (IOCON_FUNC7 | IOCON_FASTSLEW_EN) },	// LCD data pin D17 / B1
		{ 2, 12, (IOCON_FUNC7 | IOCON_FASTSLEW_EN) },	// LCD data pin D18 / B2
		{ 2, 13, (IOCON_FUNC7 | IOCON_FASTSLEW_EN) },	// LCD data pin D19 / B3
		{ 1, 26, (IOCON_FUNC7 | IOCON_FASTSLEW_EN) },	// LCD data pin D20 / B4
		{ 1, 27, (IOCON_FUNC7 | IOCON_FASTSLEW_EN) },	// LCD data pin D21 / B5
		{ 1, 28, (IOCON_FUNC7 | IOCON_FASTSLEW_EN) },	// LCD data pin D22 / B6
		{ 1, 29, (IOCON_FUNC7 | IOCON_FASTSLEW_EN) },	// LCD data pin D23 / B7

		/*Support and Control pin for LCD*/
		{ 2, 0, (IOCON_FUNC7 | IOCON_FASTSLEW_EN) },	// LCD_PWR
		{ 2, 1, (IOCON_FUNC0 | IOCON_MODE_INACT) },		// LCD Back Light Enable GPIO
		{ 2, 2, (IOCON_FUNC7 | IOCON_FASTSLEW_EN) },	// LCD D Clock
		{ 2, 3, (IOCON_FUNC7 | IOCON_FASTSLEW_EN) },	// LCD_FP -- VSYNC
		{ 2, 4, (IOCON_FUNC7 | IOCON_FASTSLEW_EN) },	// LCD_ENAB_M -- LCDDEN
		{ 2, 5, (IOCON_FUNC7 | IOCON_FASTSLEW_EN) }		// LCD_LP -- HSYNC
		//{ 2, 14, (IOCON_FUNC0 | IOCON_FASTSLEW_EN) },	// LCD BUSY
};

static void open1788_LCD_PinMuxing(void) {

	SDRAMInit();
	/* Setup LCD level pin muxing */
	Chip_IOCON_SetPinMuxing(LPC_IOCON, LCDpinmuxing,
			sizeof(LCDpinmuxing) / sizeof(PINMUX_GRP_T));
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 2, 1);// LCD Back Light Enable pin as output
	Chip_GPIO_SetPinOutHigh(LPC_GPIO, 2, 1);// LCD Back Light Enable pin set as high
	Chip_GPIO_SetPinDIR(LPC_GPIO, 2, 15, 0);// Touch Panel PENIRQ pin as input
	LCD_BLED_OUTPUT();
}

void open1788_LCDinit(void) {
	plcd = &lcd;
	plcd->disp_x_size = C_GLCD_H_SIZE;
	plcd->disp_y_size = C_GLCD_V_SIZE;
	plcd->orient = LANDSCAPE;
	plcd->display_model = LCD_TFT;

	open1788_LCD_PinMuxing();
	Chip_LCD_PowerOff(LPC_LCD);
	LCD_BLED_OFF();
	LCD_CONFIG_T LcdCfgx;
	LcdCfgx.BPP = 5;	// TFT 24bit per pixel
	LcdCfgx.HBP = C_GLCD_H_BACK_PORCH;
	LcdCfgx.HFP = C_GLCD_H_FRONT_PORCH;
	LcdCfgx.HSW = C_GLCD_H_PULSE;
	LcdCfgx.IHS = 0;
	LcdCfgx.IOE = 0;
	LcdCfgx.IPC = 0;
	LcdCfgx.IVS = 0;
	LcdCfgx.LCD = LCD_TFT;
	LcdCfgx.LPP = C_GLCD_V_SIZE;
	LcdCfgx.PPL = C_GLCD_H_SIZE;
	LcdCfgx.VBP = C_GLCD_V_BACK_PORCH;
	LcdCfgx.VFP = C_GLCD_V_FRONT_PORCH;
	LcdCfgx.VSW = C_GLCD_V_PULSE;
	LcdCfgx.color_format = LCD_COLOR_FORMAT_RGB;
	//LcdCfgx.color_format = LCD_COLOR_FORMAT_BGR;

	//Turn on LCD clock
	Chip_LCD_Init(LPC_LCD, &LcdCfgx);
	LPC_LCD->LPBASE = (uint32_t) LCD_VRAM_BASE_ADDR;
	LPC_LCD->UPBASE = (uint32_t) LCD_VRAM_BASE_ADDR;
	Chip_LCD_Cursor_Config(LPC_LCD, LCD_CURSOR_64x64, 0);
	// little endian byte order
	LPC_LCD->CTRL &= ~(1 << 9);
	// little endian pix order
	LPC_LCD->CTRL &= ~(1 << 10);
	// disable power
	LPC_LCD->CTRL &= ~(1 << 11);
	// init pixel clock
	LPC_SYSCTL->LCD_CFG = (Chip_Clock_GetPeripheralClockRate()
			/ ((unsigned long) C_GLCD_PIX_CLK));
	// bypass internal clk divider
	LPC_LCD->POL |= (1 << 26);

	Chip_LCD_PowerOn(LPC_LCD);
	Chip_LCD_Enable(LPC_LCD);
	open1788_LCD_Clear(Black);
	LCD_BLED_ON();

	touch_init();	// init touch screen
}

/*************************************************************************
 * Function Name: GLCD_SetPallet
 * Parameters: const unsigned long * pPallete
 *
 * Return: none
 *
 * Description: GLCD init colour pallete
 *
 *************************************************************************/
void open1788_LCD_SetPallet(const unsigned long * pPallete) {
	unsigned long i;
	unsigned long * pDst = (unsigned long *) LPC_LCD->PAL;
	// assert(pPallete);
	for (i = 0; i < 128; i++) {
		*pDst++ = *pPallete++;
	}
}

/******************************************************************************
 * Function Name  : open1788_LCD_SetPoint
 * Description    :
 * Input          : - Xpos: Row Coordinate
 *                  - Ypos: Line Coordinate
 * Output         : None
 * Return         : None
 * Attention		 : None
 *******************************************************************************/
void open1788_LCD_SetPoint(uint16_t Xpos, uint16_t Ypos, uint32_t color) {
	unsigned long *pDst = (unsigned long *) LCD_VRAM_BASE_ADDR;
	unsigned long value;

	if (Xpos >= C_GLCD_H_SIZE || Ypos >= C_GLCD_V_SIZE) {
		return;
	}

	value = Ypos * C_GLCD_H_SIZE + Xpos;
	pDst = pDst + value;
	*pDst = color;
}

/*************************************************************************
 * Function Name: open1788_LCD_Clear
 * Parameters: uint32_t color
 *
 * Return: none
 *
 * Description: LCD screen clearing
 *
 *************************************************************************/
void open1788_LCD_Clear(uint32_t color) {
	unsigned long i;
	unsigned long *pDst = (unsigned long *) LCD_VRAM_BASE_ADDR;

	for (i = 0; (C_GLCD_H_SIZE * C_GLCD_V_SIZE) > i; i++) {
		*pDst++ = color;
	}
}

/******************************************************************************
 * Function Name  : open1788_LCD_DrawLine
 * Description    : Bresenham's line algorithm
 * Input          : - x0:
 *                  - y0:
 *       				   - x1:
 *       		       - y1:
 *                  - color:
 * Output         : None
 * Return         : None
 * Attention		 : None
 *******************************************************************************/
void open1788_LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color) {
	unsigned int i1, j1, k1, l1;
	int n1 = 0;
	unsigned char temp = 0;
	if (y2 < y1) {
		i1 = x1;
		l1 = y1;
		x1 = x2;
		y1 = y2;
		x2 = i1;
		y2 = l1;
	}
	k1 = y2 - y1;
	if (k1 == 0) {
		if (x1 > x2) {
			i1 = x1;
			x1 = x2;
			x2 = i1;
		}
		for (i1 = x1; i1 <= x2; i1++)
			open1788_LCD_SetPoint(i1, y1, color);
	} else {
		if (x2 >= x1) {
			temp = 1;
			j1 = x2 - x1;
		} else
			j1 = x1 - x2;
		i1 = x1;
		l1 = y1;
		open1788_LCD_SetPoint(i1, l1, color);
		if (temp && (k1 <= j1))
			while (i1 != x2) {
				if (n1 < 0) {
					i1 = i1 + 1;
					n1 = n1 + (y2 - l1);
				} else {
					i1 = i1 + 1;
					l1 = l1 + 1;
					n1 = n1 + (y2 - l1) - (x2 - i1);
				}
				open1788_LCD_SetPoint(i1, l1, color);
			}
		else if (temp && (k1 > j1))
			while (l1 != y2) {
				j1 = x2 - i1;
				k1 = y2 - l1;
				if (n1 < 0) {
					i1 = i1 + 1;
					l1 = l1 + 1;
					n1 = n1 + k1 - j1;
				} else {
					l1 = l1 + 1;
					n1 = n1 - j1;
				}
				open1788_LCD_SetPoint(i1, l1, color);
			}
		else if (!temp && (k1 <= j1))
			while (i1 != x2) {
				j1 = i1 - x2;
				k1 = y2 - l1;
				if (n1 < 0) {
					i1 = i1 - 1;
					n1 = n1 + k1;
				} else {
					i1 = i1 - 1;
					l1 = l1 + 1;
					n1 = n1 + k1 - j1;
				}
				open1788_LCD_SetPoint(i1, l1, color);
			}
		else if (!temp && (k1 > j1))
			while (l1 != y2) {
				j1 = i1 - x2;
				k1 = y2 - l1;
				if (n1 < 0) {
					i1 = i1 - 1;
					l1 = l1 + 1;
					n1 = n1 + k1 - j1;
				} else {
					l1 = l1 + 1;
					n1 = n1 - j1;
				}
				open1788_LCD_SetPoint(i1, l1, color);
			}
	}
}
/******************************************************************************
 * Function Name  : open1788_LCD_Drawcircle
 * Description    : Draw round
 * Input
 * int x,         : - x0:
 * int y,         : - y0:
 * int r,      	 : - radius:
 * int color      : - color:
 * Output         : None
 * Return         : None
 * Attention		 : None
 *******************************************************************************/

void open1788_LCD_Drawcircle(int x0, int y0, int r, int color) {
	int draw_x0, draw_y0;                        // ¹ôÍ¼µã×ø±ê±äÁ¿
	int draw_x1, draw_y1;
	int draw_x2, draw_y2;
	int draw_x3, draw_y3;
	int draw_x4, draw_y4;
	int draw_x5, draw_y5;
	int draw_x6, draw_y6;
	int draw_x7, draw_y7;
	int xx, yy;                                        // »­Ô²¿ØÖÆ±äÁ¿

	int di;                                                // ¾ö²ß±äÁ¿

	/* ²ÎÊý¹ýÂË */
	if (0 == r)
		return;

	/* ¼ÆËã³ö8¸öÌØÊâµã(0¡¢45¡¢90¡¢135¡¢180¡¢225¡¢270¶È)£¬½øÐÐÏÔÊ¾ */
	draw_x0 = draw_x1 = x0;
	draw_y0 = draw_y1 = y0 + r;
	if (draw_y0 < C_GLCD_V_SIZE)
		open1788_LCD_SetPoint(draw_x0, draw_y0, color);        // 90¶È

	draw_x2 = draw_x3 = x0;
	draw_y2 = draw_y3 = y0 - r;
	if (draw_y2 >= 0)
		open1788_LCD_SetPoint(draw_x2, draw_y2, color);                 // 270¶È

	draw_x4 = draw_x6 = x0 + r;
	draw_y4 = draw_y6 = y0;
	if (draw_x4 < C_GLCD_H_SIZE)
		open1788_LCD_SetPoint(draw_x4, draw_y4, color);        // 0¶È

	draw_x5 = draw_x7 = x0 - r;
	draw_y5 = draw_y7 = y0;
	if (draw_x5 >= 0)
		open1788_LCD_SetPoint(draw_x5, draw_y5, color);                 // 180¶È
	if (1 == r)
		return;                                        // Èô°ë¾¶Îª1£¬ÔòÒÑÔ²»­Íê

	/* Ê¹ÓÃBresenham·¨½øÐÐ»­Ô² */
	di = 3 - 2 * r;                                        // ³õÊ¼»¯¾ö²ß±äÁ¿

	xx = 0;
	yy = r;
	while (xx < yy) {
		if (di < 0) {
			di += 4 * xx + 6;
		} else {
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

		/* ÒªÅÐ¶Ïµ±Ç°µãÊÇ·ñÔÚÓÐÐ§·¶Î§ÄÚ */
		if ((draw_x0 <= C_GLCD_H_SIZE) && (draw_y0 >= 0)) {
			open1788_LCD_SetPoint(draw_x0, draw_y0, color);
		}
		if ((draw_x1 >= 0) && (draw_y1 >= 0)) {
			open1788_LCD_SetPoint(draw_x1, draw_y1, color);
		}
		if ((draw_x2 <= C_GLCD_H_SIZE) && (draw_y2 <= C_GLCD_V_SIZE)) {
			open1788_LCD_SetPoint(draw_x2, draw_y2, color);
		}
		if ((draw_x3 >= 0) && (draw_y3 <= C_GLCD_V_SIZE)) {
			open1788_LCD_SetPoint(draw_x3, draw_y3, color);
		}
		if ((draw_x4 <= C_GLCD_H_SIZE) && (draw_y4 >= 0)) {
			open1788_LCD_SetPoint(draw_x4, draw_y4, color);
		}
		if ((draw_x5 >= 0) && (draw_y5 >= 0)) {
			open1788_LCD_SetPoint(draw_x5, draw_y5, color);
		}
		if ((draw_x6 <= C_GLCD_H_SIZE) && (draw_y6 <= C_GLCD_V_SIZE)) {
			open1788_LCD_SetPoint(draw_x6, draw_y6, color);
		}
		if ((draw_x7 >= 0) && (draw_y7 <= C_GLCD_V_SIZE)) {
			open1788_LCD_SetPoint(draw_x7, draw_y7, color);
		}
	}
}

/******************************************************************************
 * Function Name  : lcd_display_coord
 * Description    :
 * Input          : - Xpos:
 *                  - Ypos:
 * uint16_t color     color
 * uint16_t color1    color
 * Output         : None
 * Return         : None
 * Attention		 : None
 *******************************************************************************/
void open1788_LCD_display_coord(uint16_t x, uint16_t y, uint32_t color,	uint32_t color1) {
	uint16_t dx, dy;
	uint16_t dx1, dy1;
	uint16_t i;
	dx = x;
	dy = y;
	dx1 = x;
	dy1 = y;
	for (i = 0; i < 40; i++) {
		open1788_LCD_SetPoint(dx - 20, dy, color);
		dx++;
		open1788_LCD_SetPoint(dx1, dy1 - 20, color);
		dy1++;
	}
	dx = x;
	dy = y;
	dx1 = x;
	dy1 = y;
	for (i = 0; i < 10; i++) {
		open1788_LCD_SetPoint(dx - 20, dy - 20, color1);
		open1788_LCD_SetPoint(dx - 20, dy + 20, color1);
		dx++;
		open1788_LCD_SetPoint(dx1 - 20, dy1 - 20, color1);
		open1788_LCD_SetPoint(dx1 + 20, dy1 - 20, color1);
		dy1++;
	}
	dx = x;
	dy = y;
	dx1 = x;
	dy1 = y;
	for (i = 0; i < 10; i++) {
		open1788_LCD_SetPoint(dx + 10, dy - 20, color1);
		open1788_LCD_SetPoint(dx + 10, dy + 20, color1);

		dx++;
		open1788_LCD_SetPoint(dx1 - 20, dy1 + 10, color1);
		open1788_LCD_SetPoint(dx1 + 20, dy1 + 10, color1);
		dy1++;
	}

}

void open1788_LCD_printChar(byte c, int x, int y) {
	byte i, ch, n;
	uint16_t j;
	uint8_t* ptemp;

	ptemp = (cfont.font + ((c - cfont.offset) * cfont.y_size * cfont.Hrows));

	for (j = 0; j < (cfont.y_size); j++) {
		for (i = 0; i < cfont.Hrows; i++) {
			ch = *(ptemp + (j * cfont.Hrows) + i);
			for (n = 0; n < 8; n++) {
				if ((ch & (1 << (7 - n))) != 0) {
					open1788_LCD_SetPoint(x + n + (8 * i), y + j, plcd->fgcolor);
				} else {
					open1788_LCD_SetPoint(x + n + (8 * i), y + j, plcd->bgcolor);
				}
			}
		}
	}
}

void open1788_LCD_rotateChar(byte c, int x, int y, int pos, int deg)
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

				//tft32_setXY(newx,newy,newx+1,newy+1);

				if((ch&(1<<(7-i)))!=0)
				{
					open1788_LCD_SetPoint(newx , newy , plcd->fgcolor);
				}
				else
				{
					open1788_LCD_SetPoint(newx , newy , plcd->bgcolor);
				}
			}
		}
		temp+=(cfont.x_size/8);
	}
}

/*
 * Print a string buffer terminated in NULL or 0
 */
void open1788_LCD_print(unsigned char *st, int x, int y, int deg) {
	int stl, i;

	stl = strlen(st);

	if (plcd->orient == PORTRAIT) {
		if (x == RIGHT)
			x = plcd->disp_y_size - (stl * cfont.x_size);
		if (x == CENTER)
			x = (plcd->disp_y_size - (stl * cfont.x_size)) / 2;
	} else {
		if (x == RIGHT)
			x = (plcd->disp_y_size + 1) - (stl * cfont.x_size);
		if (x == CENTER)
			x = ((plcd->disp_y_size + 1) - (stl * cfont.x_size)) / 2;
	}

	for (i = 0; i < stl; i++)
		if (deg == 0)
			open1788_LCD_printChar(*st++, x + (i * (cfont.x_size)), y);
		else
			open1788_LCD_rotateChar(*st++, x, y, i, deg);
}

void open1788_LCD_printNumI(long num, int x, int y) {
	char buf[25];
	char st[27];
	Bool neg = FALSE;
	int i, c = 0;

	if (num == 0) {
		st[0] = 48;
		st[1] = 0;
	} else {
		if (num < 0) {
			neg = true;
			num = -num;
		}

		while (num > 0) {
			buf[c] = 48 + (num % 10);
			c++;
			num = (num - (num % 10)) / 10;
		}
		buf[c] = 0;

		if (neg) {
			st[0] = 45;
		}

		for (i = 0; i < c; i++) {
			st[i + neg] = buf[c - i - 1];
		}
		st[c + neg] = 0;
	}

	open1788_LCD_print(st, x, y, 0);
}

void open1788_LCD_printNumF(double num, byte dec, int x, int y) {
	char buf[25];
	char st[27];
	Bool neg = FALSE;
	int c = 0;
	int c2, i;
	unsigned long inum;

	if (num == 0) {
		st[0] = 48;
		st[1] = 46;
		for (i = 0; i < dec; i++)
			st[2 + i] = 48;
		st[2 + dec] = 0;
	} else {
		if (num < 0) {
			neg = true;
			num = -num;
		}

		if (dec < 1)
			dec = 1;
		if (dec > 5)
			dec = 5;

		inum = (long) (num * pow(10, dec));

		while (inum > 0) {
			buf[c] = 48 + (inum % 10);
			c++;
			inum = (inum - (inum % 10)) / 10;
		}
		if ((num < 1) && (num > 0)) {
			buf[c] = 48;
			c++;
		}
		buf[c] = 0;

		if (neg) {
			st[0] = 45;
		}

		c2 = neg;
		for (i = 0; i < c; i++) {
			st[c2] = buf[c - i - 1];
			c2++;
			if ((c - (c2 - neg)) == dec) {
				st[c2] = 46;
				c2++;
			}
		}
		st[c2] = 0;
	}

	open1788_LCD_print(st, x, y, 0);
}

/*
 * Set current font used to display characters
 * font = font array that represents letters and numbers
 * font_param = parameters that represents size, numbers of chars
 * and offset from first char
 */
void open1788_LCD_setFont(const uint8_t* font, const uint8_t* font_param) {
	cfont.font = (uint8_t*) font;
	cfont.font_param = (uint8_t*) font_param;
	cfont.x_size = font_param[0];
	cfont.y_size = font_param[1];
	cfont.offset = font_param[2];
	cfont.numchars = font_param[3];
	cfont.Hrows = cfont.x_size / 8;
	if (cfont.x_size % 8)
		cfont.Hrows++;
}

void open1788_LCD_setColor(uint32_t color) {
	plcd->fgcolor = color;
}

void open1788_LCD_setBackColor(uint32_t color) {
	plcd->bgcolor = color;
}

int open1788_LCD_getDisplayXSize(void) {
	if (plcd->orient == PORTRAIT)
		return plcd->disp_x_size + 1;
	else
		return plcd->disp_y_size + 1;
}

int open1788_LCD_getDisplayYSize(void) {
	if (plcd->orient == PORTRAIT)
		return plcd->disp_y_size + 1;
	else
		return plcd->disp_x_size + 1;
}

uint8_t* open1788_LCD_getFont(void) {
	return cfont.font;
}

uint8_t* open1788_LCD_getFont_param(void) {
	return cfont.font_param;
}

uint8_t open1788_LCD_getFontXsize(void) {
	return cfont.x_size;
}

uint8_t open1788_LCD_getFontYsize(void) {
	return cfont.y_size;
}

uint16_t open1788_LCD_getColor(void) {
	return plcd->fgcolor;
}

uint16_t open1788_LCD_getBackColor(void) {
	return plcd->bgcolor;
}


void open1788_LCD_drawBitmap_(int x, int y, int sx, int sy, uint16_t* data, int scale)
{
	uint16_t col;
	int tx, ty, tc, tsx, tsy;
	//byte r, g, b;

	if (scale==1)
	{
		if (plcd->orient==PORTRAIT)
		{
			//tft32_setXY(x, y, x+sx-1, y+sy-1);
			for (tc=0; tc<(sx*sy); tc++)
			{
				col=data[tc];
				open1788_LCD_SetPoint(x, y, col);			}
		}
		else
		{
			for (ty=0; ty<sy; ty++)
			{
				//tft32_setXY(x, y+ty, x+sx-1, y+ty);
				for (tx=sx-1; tx>=0; tx--)
				{
					col=data[(ty*sx)+tx];
					open1788_LCD_SetPoint(x+tx, y+ty, col);				}
			}
		}
	}
	else
	{
		if (plcd->orient==PORTRAIT)
		{
			for (ty=0; ty<sy; ty++)
			{
				//tft32_setXY(x, y+(ty*scale), x+((sx*scale)-1), y+(ty*scale)+scale);
				for (tsy=0; tsy<scale; tsy++)
					for (tx=0; tx<sx; tx++)
					{
						col=data[(ty*sx)+tx];
						for (tsx=0; tsx<scale; tsx++)
							open1788_LCD_SetPoint(x+tsx, tsy, col);					}
			}
		}
		else
		{
			for (ty=0; ty<sy; ty++)
			{
				for (tsy=0; tsy<scale; tsy++)
				{
					//tft32_setXY(x, y+(ty*scale)+tsy, x+((sx*scale)-1), y+(ty*scale)+tsy);
					for (tx=sx-1; tx>=0; tx--)
					{
						col=data[(ty*sx)+tx];
						for (tsx=0; tsx<scale; tsx++)
							open1788_LCD_SetPoint(x+tsx, y+tsy, col);					}
				}
			}
		}
	}
}


void open1788_LCD_drawBitmap(int x, int y, int sx, int sy, uint16_t* data, int deg, int rox, int roy)
{
	unsigned int col;
	int tx, ty, newx, newy;
	//byte r, g, b;
	double radian;
	radian=deg*0.0175;

	if (deg==0)
		open1788_LCD_drawBitmap_(x, y, sx, sy, data,1);
	else
	{
		for (ty=0; ty<sy; ty++)
		{
			for (tx=0; tx<sx; tx++)
			{
				col=data[(ty*sx)+tx];

				newx=x+rox+(((tx-rox)*cos(radian))-((ty-roy)*sin(radian)));
				newy=y+roy+(((ty-roy)*cos(radian))+((tx-rox)*sin(radian)));

				//tft32_setXY(newx, newy, newx, newy);
				open1788_LCD_SetPoint(newx, newy, col);
			}
		}
	}
}

void open1788_LCD_drawHLine(int x, int y, int l)
{
    int i;

    for (i = 0; i < l + 1; i++)
    {
    	open1788_LCD_SetPoint(x+i, y, plcd->fgcolor);
    }
}

void open1788_LCD_drawVLine(int x, int y, int l)
{
    int i;

    for (i = 0; i < l; i++)
    {
    	open1788_LCD_SetPoint(x, y+i, plcd->fgcolor);
    }
}

void open1788_LCD_drawRect(int x1, int y1, int x2, int y2)
{
    int tmp;

    if (x1 > x2)
    {
        tmp = x1;
        x1 = x2;
        x2 = tmp;
    }
    if (y1 > y2)
    {
        tmp = y1;
        y1 = y2;
        y2 = tmp;
    }

    open1788_LCD_drawHLine(x1, y1, x2 - x1);
    open1788_LCD_drawHLine(x1, y2, x2 - x1);
    open1788_LCD_drawVLine(x1, y1, y2 - y1);
    open1788_LCD_drawVLine(x2, y1, y2 - y1);
}

void open1788_LCD_drawRoundRect(int x1, int y1, int x2, int y2)
{
    int tmp;

    if (x1 > x2)
    {
        tmp = x1;
        x1 = x2;
        x2 = tmp;
    }
    if (y1 > y2)
    {
        tmp = y1;
        y1 = y2;
        y2 = tmp;
    }
    if ((x2 - x1) > 4 && (y2 - y1) > 4)
    {
    	open1788_LCD_SetPoint(x1 + 1, y1 + 1, plcd->fgcolor);
    	open1788_LCD_SetPoint(x2 - 1, y1 + 1, plcd->fgcolor);
    	open1788_LCD_SetPoint(x1 + 1, y2 - 1, plcd->fgcolor);
    	open1788_LCD_SetPoint(x2 - 1, y2 - 1, plcd->fgcolor);
    	open1788_LCD_drawHLine(x1 + 2, y1, x2 - x1 - 4);
    	open1788_LCD_drawHLine(x1 + 2, y2, x2 - x1 - 4);
    	open1788_LCD_drawVLine(x1, y1 + 2, y2 - y1 - 4);
    	open1788_LCD_drawVLine(x2, y1 + 2, y2 - y1 - 4);
    }
}

void open1788_LCD_fillRect(int x1, int y1, int x2, int y2)
{
    int tmp, i;

    if (x1 > x2)
    {
        tmp = x1;
        x1 = x2;
        x2 = tmp;
    }
    if (y1 > y2)
    {
        tmp = y1;
        y1 = y2;
        y2 = tmp;
    }

    if (plcd->orient == PORTRAIT)
    {
        for (i = 0; i < ((y2 - y1) / 2) + 1; i++)
        {
        	open1788_LCD_drawHLine(x1, y1 + i, x2 - x1);
        	open1788_LCD_drawHLine(x1, y2 - i, x2 - x1);
        }
    }
    else
    {
        for (i = 0; i < ((x2 - x1) / 2) + 1; i++)
        {
        	open1788_LCD_drawVLine(x1 + i, y1, y2 - y1);
        	open1788_LCD_drawVLine(x2 - i, y1, y2 - y1);
        }
    }
}

void open1788_LCD_fillRoundRect(int x1, int y1, int x2, int y2)
{
    int tmp, i;

    if (x1 > x2)
    {
        tmp = x1;
        x1 = x2;
        x2 = tmp;
    }
    if (y1 > y2)
    {
        tmp = y1;
        y1 = y2;
        y2 = tmp;
    }

    if ((x2 - x1) > 4 && (y2 - y1) > 4)
    {
        for (i = 0; i < ((y2 - y1) / 2) + 1; i++)
        {
            switch (i)
            {
            case 0:
            	open1788_LCD_drawHLine(x1 + 2, y1 + i, x2 - x1 - 4);
            	open1788_LCD_drawHLine(x1 + 2, y2 - i, x2 - x1 - 4);
                break;
            case 1:
            	open1788_LCD_drawHLine(x1 + 1, y1 + i, x2 - x1 - 2);
            	open1788_LCD_drawHLine(x1 + 1, y2 - i, x2 - x1 - 2);
                break;
            default:
            	open1788_LCD_drawHLine(x1, y1 + i, x2 - x1);
            	open1788_LCD_drawHLine(x1, y2 - i, x2 - x1);
            }
        }
    }
}

/*************************************************************************
 * Function Name: GLCD_Ctrl
 * Parameters: Bool bEna
 *
 * Return: none
 *
 * Description: GLCD enable disabe sequence
 *
 *************************************************************************/
void open1788_LCD_Ctrl (Bool bEna)
{
	volatile uint32_t i;
  if (bEna)
  {
    LPC_LCD->CTRL |= (1<<0);
    for(i = C_GLCD_PWR_ENA_DIS_DLY; i; i--)
    LPC_LCD->CTRL |= (1<<11);
  }
  else
  {
    LPC_LCD->CTRL &= ~(1<<11);
    for(i = C_GLCD_PWR_ENA_DIS_DLY; i; i--);
    LPC_LCD->CTRL &= ~(1<<0);
  }
}
