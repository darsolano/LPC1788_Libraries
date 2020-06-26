/*
 * ssd1289.h
 *
 *  Created on: Oct 7, 2015
 *      Author: dsolano
 */

#ifndef INCLUDE_SSD1289_H_
#define INCLUDE_SSD1289_H_

#include <chip.h>

/*
  ITDB02_Graph16.h - Arduino library support for 16bit (W)QVGA LCD Boards
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

/* Display coordinates:
 *  [x,y]
 *     [0,319]                                 [0,0]
 *       -----------------------------------------
 *       |                                       |
 *       |                                       |
 *       |                                       |
 *       |                                       |
 *       |             TOUCH DISPLAY             |
 *       |                                       |
 *       |                                       |
 *       |                                       |
 *       |                                       |
 *       -----------------------------------------
 *   [239,319]                               [239,0]
 */

#define LEFT 0
#define RIGHT 9999
#define CENTER 9998

#define PORTRAIT 0
#define LANDSCAPE 1
#define TFT_MASK	0x0fffffff

#define X_SIZE		240
#define Y_SIZE		320

#define PORTRAIT_X_SIZE		X_SIZE
#define PORTRAIT_Y_SIZE		Y_SIZE

#define LANDSCAPE_X_SIZE	Y_SIZE
#define LANDSCPAE_Y_SIZE	X_SIZE

//*********************************
// COLORS
//*********************************
// VGA color palette
/* -------------------------------------------------------------------------------- */
/* -- µGUI COLORS                                                                -- */
/* -- Source: http://www.rapidtables.com/web/color/RGB_Color.htm                 -- */
/* -------------------------------------------------------------------------------- */
#define TFT_MAROON                       0x8000
#define TFT_DARK_RED                     0x8800
#define TFT_BROWN                        0xA145
#define TFT_FIREBRICK                    0xB104
#define TFT_CRIMSON                      0xD8A7
#define TFT_RED                          0xF800
#define TFT_TOMATO                       0xFB09
#define TFT_CORAL                        0xFBEA
#define TFT_INDIAN_RED                   0xCAEB
#define TFT_LIGHT_CORAL                  0xEC10
#define TFT_DARK_SALMON                  0xE4AF
#define TFT_SALMON                       0xF40E
#define TFT_LIGHT_SALMON                 0xFD0F
#define TFT_ORANGE_RED                   0xFA20
#define TFT_DARK_ORANGE                  0xFC60
#define TFT_ORANGE                       0xFD20
#define TFT_GOLD                         0xFEA0
#define TFT_DARK_GOLDEN_ROD              0xB421
#define TFT_GOLDEN_ROD                   0xDD24
#define TFT_PALE_GOLDEN_ROD              0xEF35
#define TFT_DARK_KHAKI                   0xBDAD
#define TFT_KHAKI                        0xEF31
#define TFT_OLIVE                        0x8400
#define TFT_YELLOW                       0xFFE0
#define TFT_YELLOW_GREEN                 0x9E66
#define TFT_DARK_OLIVE_GREEN             0x5346
#define TFT_OLIVE_DRAB                   0x6C64
#define TFT_LAWN_GREEN                   0x7FC0
#define TFT_CHART_REUSE                  0x7FE0
#define TFT_GREEN_YELLOW                 0xAFE6
#define TFT_DARK_GREEN                   0x0320
#define TFT_GREEN                        0x07E0
#define TFT_FOREST_GREEN                 0x2444
#define TFT_LIME                         0x07E0
#define TFT_LIME_GREEN                   0x3666
#define TFT_LIGHT_GREEN                  0x9772
#define TFT_PALE_GREEN                   0x97D2
#define TFT_DARK_SEA_GREEN               0x8DD1
#define TFT_MEDIUM_SPRING_GREEN          0x07D3
#define TFT_SPRING_GREEN                 0x07EF
#define TFT_SEA_GREEN                    0x344B
#define TFT_MEDIUM_AQUA_MARINE           0x6675
#define TFT_MEDIUM_SEA_GREEN             0x3D8E
#define TFT_LIGHT_SEA_GREEN              0x2595
#define TFT_DARK_SLATE_GRAY              0x328A
#define TFT_TEAL                         0x0410
#define TFT_DARK_CYAN                    0x0451
#define TFT_AQUA                         0x07FF
#define TFT_CYAN                         0x07FF
#define TFT_LIGHT_CYAN                   0xDFFF
#define TFT_DARK_TURQUOISE               0x0679
#define TFT_TURQUOISE                    0x46F9
#define TFT_MEDIUM_TURQUOISE             0x4E99
#define TFT_PALE_TURQUOISE               0xAF7D
#define TFT_AQUA_MARINE                  0x7FFA
#define TFT_POWDER_BLUE                  0xAEFC
#define TFT_CADET_BLUE                   0x64F3
#define TFT_STEEL_BLUE                   0x4C16
#define TFT_CORN_FLOWER_BLUE             0x64BD
#define TFT_DEEP_SKY_BLUE                0x05FF
#define TFT_DODGER_BLUE                  0x249F
#define TFT_LIGHT_BLUE                   0xAEBC
#define TFT_SKY_BLUE                     0x867D
#define TFT_LIGHT_SKY_BLUE               0x867E
#define TFT_MIDNIGHT_BLUE                0x18CE
#define TFT_NAVY                         0x0010
#define TFT_DARK_BLUE                    0x0011
#define TFT_MEDIUM_BLUE                  0x0019
#define TFT_BLUE                         0x001F
#define TFT_ROYAL_BLUE                   0x435B
#define TFT_BLUE_VIOLET                  0x897B
#define TFT_INDIGO                       0x4810
#define TFT_DARK_SLATE_BLUE              0x49F1
#define TFT_SLATE_BLUE                   0x6AD9
#define TFT_MEDIUM_SLATE_BLUE            0x7B5D
#define TFT_MEDIUM_PURPLE                0x939B
#define TFT_DARK_MAGENTA                 0x8811
#define TFT_DARK_VIOLET                  0x901A
#define TFT_DARK_ORCHID                  0x9999
#define TFT_MEDIUM_ORCHID                0xBABA
#define TFT_PURPLE                       0x8010
#define TFT_THISTLE                      0xD5FA
#define TFT_PLUM                         0xDD1B
#define TFT_VIOLET                       0xEC1D
#define TFT_MAGENTA                      0xF81F
#define TFT_ORCHID                       0xDB9A
#define TFT_MEDIUM_VIOLET_RED            0xC0B0
#define TFT_PALE_VIOLET_RED              0xDB92
#define TFT_DEEP_PINK                    0xF8B2
#define TFT_HOT_PINK                     0xFB56
#define TFT_LIGHT_PINK                   0xFDB7
#define TFT_PINK                         0xFDF9
#define TFT_ANTIQUE_WHITE                0xF75A
#define TFT_BEIGE                        0xF7BB
#define TFT_BISQUE                       0xFF18
#define TFT_BLANCHED_ALMOND              0xFF59
#define TFT_WHEAT                        0xF6F6
#define TFT_CORN_SILK                    0xFFBB
#define TFT_LEMON_CHIFFON                0xFFD9
#define TFT_LIGHT_GOLDEN_ROD_YELLOW      0xF7DA
#define TFT_LIGHT_YELLOW                 0xFFFB
#define TFT_SADDLE_BROWN                 0x8A22
#define TFT_SIENNA                       0x9A85
#define TFT_CHOCOLATE                    0xD344
#define TFT_PERU                         0xCC28
#define TFT_SANDY_BROWN                  0xF52C
#define TFT_BURLY_WOOD                   0xDDB0
#define TFT_TAN                          0xD591
#define TFT_ROSY_BROWN                   0xBC71
#define TFT_MOCCASIN                     0xFF16
#define TFT_NAVAJO_WHITE                 0xFEF5
#define TFT_PEACH_PUFF                   0xFED6
#define TFT_MISTY_ROSE                   0xFF1B
#define TFT_LAVENDER_BLUSH               0xFF7E
#define TFT_LINEN                        0xF77C
#define TFT_OLD_LACE                     0xFFBC
#define TFT_PAPAYA_WHIP                  0xFF7A
#define TFT_SEA_SHELL                    0xFFBD
#define TFT_MINT_CREAM                   0xF7FE
#define TFT_SLATE_GRAY                   0x7412
#define TFT_LIGHT_SLATE_GRAY             0x7453
#define TFT_LIGHT_STEEL_BLUE             0xAE1B
#define TFT_LAVENDER                     0xE73E
#define TFT_FLORAL_WHITE                 0xFFDD
#define TFT_ALICE_BLUE                   0xEFBF
#define TFT_GHOST_WHITE                  0xF7BF
#define TFT_HONEYDEW                     0xEFFD
#define TFT_IVORY                        0xFFFD
#define TFT_AZURE                        0xEFFF
#define TFT_SNOW                         0xFFDE
#define TFT_BLACK                        0x0000
#define TFT_DIM_GRAY                     0x6B4D
#define TFT_GRAY                         0x8410
#define TFT_DARK_GRAY                    0xAD55
#define TFT_SILVER                       0xBDF7
#define TFT_LIGHT_GRAY                   0xD69A
#define TFT_GAINSBORO                    0xDEDB
#define TFT_WHITE_SMOKE                  0xF7BE
#define TFT_WHITE                        0xFFFF


//*********************************
// COLORS
//*********************************
// VGA color palette
#define VGA_BLACK		0x0000
#define VGA_WHITE		0xFFFF
#define VGA_RED			0xF800
#define VGA_GREEN		0x0400
#define VGA_BLUE		0x001F
#define VGA_SILVER		0xC618
#define VGA_GRAY		0x8410
#define VGA_MAROON		0x8000
#define VGA_YELLOW		0xFFE0
#define VGA_OLIVE		0x8400
#define VGA_LIME		0x07E0
#define VGA_AQUA		0x07FF
#define VGA_TEAL		0x0410
#define VGA_NAVY		0x0010
#define VGA_FUCHSIA		0xF81F
#define VGA_PURPLE		0x8010
#define VGA_TRANSPARENT	0xFFFFFFFF

#define KIND_OF_BLUE	0x3D1F
#define WHITE           0xFFFF
#define GREEN           0x07E0
#define RED             0xF800
#define BLUE            0x001F
#define GREY            0xF7DE
#define ORANGE          0xFA20
#define YELLOW          0xFFE0
#define GRAY           	0xF7DE
#define LIGHTBLUE       0x051F
#define	MAGENTA_1       0xF81F
#define NAVY         	0x000F
#define DGREEN      	0x03E0
#define DCYAN       	0x03EF
#define MAROON       	0x7800
#define PURPLE       	0x780F
#define OLIVE        	0x7BE0
#define LGRAY        	0xC618
#define DGRAY        	0x7BEF
#define CYAN         	0x07FF
#define RED          	0xF800
#define VERY_DARK_GRAY	0x528A

/* Black color, 565 mode */
#define BLACK         0x0000
/* Light gray color, 565 mode */
#define LIGHTGRAY     0X7BEF
/* Dark gray color, 565 mode */
#define DARKGRAY      0x39E7
/* Light red color, 565 mode */
#define LIGHTRED      0x7800
/* Light green color, 565 mode */
#define LIGHTGREEN    0x03E0
/* Light blue color, 565 mode */
//#define LIGHTBLUE     0x000F
/* Light magenta color, 565 mode */
#define LIGHTMAGENTA  (LIGHTRED | LIGHTBLUE)
/* Light cyan color, 565 mode */
#define LIGHTCYAN     (LIGHTGREEN | LIGHTBLUE)
/* Light yellow color, 565 mode */
#define LIGHTYELLOW   (LIGHTRED | LIGHTGREEN)

#define ASSEMBLE_RGB(R ,G, B)    ((((R)& 0xF8) << 8) | (((G) & 0xFC) << 3) | (((B) & 0xF8) >> 3))
#define RGB565CONVERT(red, green, blue) (int) (((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3))

// Register definition
#define OSCILLATOR_START		0x0000
#define DRIVER_OUTPUT_CONTROL	0x0001
#define LCD_DRIVER_AC_CONTROL	0x0002
#define POWER_CONTROL_1			0x0003
#define COMPARE_REGISTER_1		0x0005
#define COMPARE_REGISTER_2		0x0006
#define DISPLAY_CONTROL			0x0007
#define FRAME_CYCLE_CONTROL		0x000B
#define POWER_CONTROL_2			0x000C
#define POWER_CONTROL_3			0x000D
#define POWER_CONTROL_4			0x000E
#define GATE_SCAN_START_POS		0x000F
#define SLEEP_MODE				0x0010
#define ENTRY_MODE				0x0011
#define HORIZONTAL_PORCH		0x0016
#define VERTICAL_PORCH			0x0017
#define POWER_CONTROL_5			0x001E
#define RAM_DATA_WRITE			0x0022
#define RAM_DATA_READ			0x0022	// with read bit
#define RAM_WRITE_DATA_MASK_1	0x0023
#define RAM_WRITE_DATA_MASK_2	0x0024
#define VCOM_OTP_1				0x0028
#define VCOM_OTP_2				0x0029
#define GAMMA_CONTROL_1			0x0030
#define GAMMA_CONTROL_2			0x0031
#define GAMMA_CONTROL_3			0x0032
#define GAMMA_CONTROL_4			0x0033
#define GAMMA_CONTROL_5			0x0034
#define GAMMA_CONTROL_6			0x0035
#define GAMMA_CONTROL_7			0x0036
#define GAMMA_CONTROL_8			0x0037
#define GAMMA_CONTROL_9			0x003A
#define GAMMA_CONTROL_10		0x003B
#define VERTICAL_SCROLL_CONTROL_1	0x0041
#define VERTICAL_SCROLL_CONTROL_2	0x0042
#define HORIZONTAL_RAM_ADDR_POS		0x0044
#define VERTICAL_RAM_ADDR_START_POS	0x0045
#define VERTICAL_RAM_ADDR_END_POS	0x0046
#define FIRST_WINDOW_START			0x0048
#define FIRST_WINDOW_END			0x0049
#define SECOND_WINDOW_START			0x004A
#define SECOND_WINDOW_END			0x004B
#define SET_GDDRAM_X_ADDR_COUNTER	0x004E
#define SET_GDDRAM_Y_ADDR_COUNTER	0x004F

typedef uint8_t	byte;

struct _current_font
{
	uint8_t* font;
	uint8_t* font_param;
	uint8_t x_size;
	uint8_t y_size;
	uint8_t offset;
	uint8_t numchars;
	uint8_t Hrows;
};
struct _current_font cfont;


void tft32_InitLCD(void);
void tft32_clrScr(uint16_t color);	// RGB565
void tft32_drawPixel(int x, int y);
void tft32_drawLine(int x1, int y1, int x2, int y2);

//void tft32_fillScr(byte r, byte g, byte b);
void tft32_fillScr(uint16_t color);
void tft32_drawRect(int x1, int y1, int x2, int y2);
void tft32_drawRoundRect(int x1, int y1, int x2, int y2);
void tft32_fillRect(int x1, int y1, int x2, int y2);
void tft32_fillRoundRect(int x1, int y1, int x2, int y2);
void tft32_drawCircle(int x, int y, int radius);
void tft32_fillCircle(int x, int y, int radius);
void tft32_setColor(uint16_t color);
void tft32_setBackColor(uint16_t color);
void tft32_print(char *st, int x, int y, int deg);
void tft32_printNumI(long num, int x, int y);
void tft32_printNumF(double num, byte dec, int x, int y);
void tft32_setFont(const uint8_t* font, const uint8_t* font_param);
void tft32_drawBitmap_(int x, int y, int sx, int sy, uint16_t* data, int scale);
void tft32_drawBitmap(int x, int y, int sx, int sy, uint16_t* data, int deg, int rox, int roy);
void tft32_LCD_Writ_Bus(uint16_t frame);
void tft32_SleepMode(void);


typedef struct
{
	uint16_t fgcolor;
	uint16_t bgcolor;
	byte orient;
	byte display_model;
	long disp_y_size , disp_x_size;
}DISPLAY_t;
DISPLAY_t tft;

void tft32_LCD_Write_COM(uint16_t cmd);
void tft32_LCD_Write_DATA(uint16_t data);
void tft32_main_W_com_data(uint16_t com1,uint16_t dat1);
void tft32_setPixel(uint16_t color);
void tft32_drawHLine(int x, int y, int l);
void tft32_drawVLine(int x, int y, int l);
void tft32_printChar(byte c, int x, int y);
void tft32_setXY(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void tft32_rotateChar(byte c, int x, int y, int pos, int deg);

void tft32_backlight_on(void);
void tft32_backlight_off(void);
void tft32_hdw_reset(void);
void tft32_PINS(void);
void tft32_set_orientation(byte orientation);
void tft32_ReleaseFromSleep(void);
int tft32_getDisplayXSize(void);
int tft32_getDisplayYSize(void);
uint8_t* tft32_getFont(void);
uint8_t* tft32_getFont_param(void);
uint8_t tft32_getFontXsize(void);
uint8_t tft32_getFontYsize(void);
uint16_t tft32_getColor(void);
uint16_t tft32_getBackColor(void);

#endif /* INCLUDE_SSD1289_H_ */


