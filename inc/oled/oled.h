/*****************************************************************************
 *   oled.h:  Header file for OLED Display
 *
 *   Copyright(C) 2009, Embedded Artists AB
 *   All rights reserved.
 *
******************************************************************************/
#ifndef __OLED_H
#define __OLED_H

#include <chip.h>
//#define OLED_USE_I2C


#define I2CDEV 			LPC_I2C1

#define OLED_DISPLAY_WIDTH  128
#ifdef OLED_USE_I2C
#define OLED_DISPLAY_HEIGHT 64
#define OLED_I2C_ADDR 	(0x3c)	// 0x78 on board after write bit inserted and shifted left
#define OLED_I2C_ID		I2C1
#define OLED_I2C_RATE	400000
#else
#define OLED_DISPLAY_HEIGHT 32
#endif

#define OLED_ALIGN_CENTER	1
#define OLED_ALIGN_RIGHT	2
#define OLED_ALIGN_LEFT		3

// ---------------------------------------------------------------------------
// SSD1306 Commands
// ---------------------------------------------------------------------------
// 1. Fundamental Command Table
#define OLED_SET_CONTRAST           0x81
#define OLED_DISP_NOR               0xA6
#define OLED_DISP_REV               0xA7
#define OLED_DISP_OFF               0xAE
#define OLED_DISP_ON                0xAF
#define OLED_EON_OFF                0xA4
#define OLED_EON_ON                 0xA5

// 2. Scrolling Command Table
#define OLED_SCROLL_RIGHT           0x26
#define OLED_SCROLL_LEFT            0x27
#define OLED_SCROLL_VR              0x29
#define OLED_SCROLL_VL              0x2A
#define OLED_SCROLL_OFF             0x2E
#define OLED_SCROLL_ON              0x2F
#define OLED_VERT_SCROLL_A          0xA3

// 3. Addressing Setting Command Table
#define OLED_SET_COL_LO             0x00
#define OLED_SET_COL_HI             0x10
#define OLED_MEM_ADDRESSING         0x20
#define OLED_SET_COL_ADDR           0x21
#define OLED_SET_PAGE_ADDR          0x22
#define OLED_SET_PAGE               0xB0

// 4. Hardware Configuration (Panel resolution & layout related) Command Table
#define OLED_SET_LINE               0x40
#define OLED_SET_SEG_REMAP0         0xA0
#define OLED_SET_SEG_REMAP1         0xA1
#define OLED_MULTIPLEX              0xA8
#define OLED_SET_SCAN_FLIP          0xC0
#define OLED_SET_SCAN_NOR           0xC8
#define OLED_SET_OFFSET             0xD3
#define OLED_SET_PADS               0xDA

// 5. Timing & Driving Scheme Setting Command Table
#define OLED_SET_RATIO_OSC          0xD5
#define OLED_SET_CHARGE             0xD9
#define OLED_SET_VCOM               0xDB
#define OLED_NOP                    0xE3



#define OLED_CHARGE_PUMP            0x8D
#define OLED_PUMP_OFF               0x10
#define OLED_PUMP_ON                0x14

typedef enum
{
    OLED_COLOR_BLACK,
    OLED_COLOR_WHITE
} oled_color_t;

typedef struct
{
	uint8_t* font;	// array with image pointer
	uint8_t  firstchar;
	int16_t  height;
	int16_t  width;
	uint16_t charamount;
	uint16_t fontHrows;
}FONTCFG_Typedef;

void oled_init (void);
void oled_putPixel(uint8_t x, uint8_t y, oled_color_t color);
void oled_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, oled_color_t color);
void oled_circle(uint8_t x0, uint8_t y0, uint8_t r, oled_color_t color);
void oled_rect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, oled_color_t color);
void oled_fillRect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, oled_color_t color);
void oled_clearScreen(oled_color_t color);
void oled_putString(uint8_t x, uint8_t y, uint8_t *pStr, oled_color_t fb,
		oled_color_t bg, FONTCFG_Typedef* pfont, uint8_t align);
uint8_t oled_putChar(uint8_t x, uint8_t y, uint8_t ch, oled_color_t fb,
		oled_color_t bg, FONTCFG_Typedef* pfont);
uint8_t oled_putChar_5x7(uint8_t x, uint8_t y, uint8_t ch, oled_color_t fb, oled_color_t bg);
Status oled_FontSetup( const uint8_t* fontarray, FONTCFG_Typedef* pfont, const uint8_t* font_param);
void oled_PutDec99(uint8_t decnum , uint8_t xpos, uint8_t ypos, oled_color_t char_color , oled_color_t bg_color, FONTCFG_Typedef *font);
void oled_PutDec(uint8_t decnum , uint8_t xpos, uint8_t ypos, oled_color_t char_color , oled_color_t bg_color, FONTCFG_Typedef *font);
void oled_PutDec16(uint16_t decnum , uint8_t xpos, uint8_t ypos, oled_color_t char_color , oled_color_t bg_color, FONTCFG_Typedef *font);


#endif /* end __OLED_H */
/****************************************************************************
**                            End Of File
*****************************************************************************/
