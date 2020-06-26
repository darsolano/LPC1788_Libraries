/*
 * File:   ssd1306.c
 * Must include ssd1306.h
 * Author: dsolano
 * I2C Driven SSD1306 or SH1106 SaintSmart OLED LCD 128x64
 * Created on July 5, 2014, 1:06 AM
 * NXP LPCXpresso 1769
 */

#ifndef SSD1306_H
#define	SSD1306_H

#include <chip.h>
#include <lpc_types.h>
#include <stdint.h>

#define PRIVATE						static

#define USE_I2C						1
#define USE_SPI						0

#if USE_I2C
#define SSD1306_I2C_ADDR            0x3C
#define SSD1306_I2C_BUS             I2C1
#define SSD1306_I2C_RATE			100000

// ---------------------------------------------------------------------------
// SSD1306 Control Byte Definition
// ---------------------------------------------------------------------------
#define CTRL_DATA_FRAME             0b01000000
#define CTRL_CMD_FRAME              0b00000000
#endif

#define OLED_DISPLAY_WIDTH          128
#if USE_I2C
#define OLED_DISPLAY_HEIGHT         64
#endif

#if USE_SPI
#define OLEDSSPx					LPC_SSP0
#define OLED_DISPLAY_HEIGHT         32
#endif

// ---------------------------------------------------------------------------
// SSD1306 Commands
// ---------------------------------------------------------------------------
// 1. Fundamental Command Table
#define GLCD_SET_CONTRAST           0x81
#define GLCD_DISP_NOR               0xA6
#define GLCD_DISP_REV               0xA7
#define GLCD_DISP_OFF               0xAE
#define GLCD_DISP_ON                0xAF
#define GLCD_EON_OFF                0xA4
#define GLCD_EON_ON                 0xA5

// 2. Scrolling Command Table
#define GLCD_SCROLL_RIGHT           0x26
#define GLCD_SCROLL_LEFT            0x27
#define GLCD_SCROLL_VR              0x29
#define GLCD_SCROLL_VL              0x2A
#define GLCD_SCROLL_OFF             0x2E
#define GLCD_SCROLL_ON              0x2F
#define GLCD_VERT_SCROLL_A          0xA3

// 3. Addressing Setting Command Table
#define GLCD_SET_COL_LO             0x00
#define GLCD_SET_COL_HI             0x10
#define GLCD_MEM_ADDRESSING         0x20
#define GLCD_SET_COL_ADDR           0x21
#define GLCD_SET_PAGE_ADDR          0x22
#define GLCD_SET_PAGE               0xB0

// 4. Hardware Configuration (Panel resolution & layout related) Command Table
#define GLCD_SET_LINE               0x40
#define GLCD_SET_SEG_REMAP0         0xA0
#define GLCD_SET_SEG_REMAP1         0xA1
#define GLCD_MULTIPLEX              0xA8
#define GLCD_SET_SCAN_FLIP          0xC0
#define GLCD_SET_SCAN_NOR           0xC8
#define GLCD_SET_OFFSET             0xD3
#define GLCD_SET_PADS               0xDA

// 5. Timing & Driving Scheme Setting Command Table
#define GLCD_SET_RATIO_OSC          0xD5
#define GLCD_SET_CHARGE             0xD9
#define GLCD_SET_VCOM               0xDB
#define GLCD_NOP                    0xE3



#define GLCD_CHARGE_PUMP            0x8D
#define GLCD_PUMP_OFF               0x10
#define GLCD_PUMP_ON                0x14
// ---------------------------------------------------------------------------
// Global Variables
// ---------------------------------------------------------------------------
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

/*
 * The display controller can handle a resolution of 132x64. The OLED
 * on the base board is 96x64.
 */
#define X_OFFSET 2

#define SHADOW_FB_SIZE (OLED_DISPLAY_WIDTH*OLED_DISPLAY_HEIGHT >> 3)

#define mSetAddress(page,lowerAddr,higherAddr)\
    GLCDWriteCommand(page);\
    GLCDWriteCommand(lowerAddr);\
    GLCDWriteCommand(higherAddr);
/*
 * The SSD1305 doesn't support reading from the display memory when using
 * serial mode (only parallel mode). Since it isn't possible to write only
 * one pixel to the display (a minimum of one column, 8 pixels, is always
 * wriiten) a shadow framebuffer is needed to keep track of the display
 * data.
 */



// ---------------------------------------------------------------------------
// Function Prototypes
// ---------------------------------------------------------------------------
void 		GLCD_UpdateScreen(void);
int 		GLCD_SSD1306Init(FunctionalState I2CINIT);
void        GLCDisplayOn(void);
void        GLCDisplayOff(void);
void        GLCD_putPixel(uint8_t x, uint8_t y, oled_color_t color);
void        GLCD_putString(uint8_t x, uint8_t y, uint8_t *pStr, oled_color_t fb, oled_color_t bg, FONTCFG_Typedef* pfont);
uint8_t 	GLCD_putChar(uint8_t x, uint8_t y, uint8_t ch, oled_color_t fb,oled_color_t bg, FONTCFG_Typedef* pfont);
void        GLCD_fillRect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, oled_color_t color);
void        GLCD_rect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, oled_color_t color);
void        GLCD_circle(uint8_t x0, uint8_t y0, uint8_t r, oled_color_t color);
void        GLCD_Line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, oled_color_t color);
void        GLCD_ClearScreen(oled_color_t color);
Status 		GLCD_FontSetup( const uint8_t* fontarray, FONTCFG_Typedef* pfont, const uint8_t* font_param);

void GLCD_PutDec99(uint8_t decnum , uint8_t xpos, uint8_t ypos, oled_color_t char_color , oled_color_t bg_color, FONTCFG_Typedef *font);
void GLCD_PutDec(uint8_t decnum , uint8_t xpos, uint8_t ypos, oled_color_t char_color , oled_color_t bg_color, FONTCFG_Typedef *font);
void GLCD_PutDec16(uint16_t decnum , uint8_t xpos, uint8_t ypos, oled_color_t char_color , oled_color_t bg_color, FONTCFG_Typedef *font);
void GLCD_PutDec32(uint32_t decnum , uint8_t xpos, uint8_t ypos, oled_color_t char_color , oled_color_t bg_color, FONTCFG_Typedef *font);
void GLCD_PutHex_(uint8_t hexnum , uint8_t xpos, uint8_t ypos, oled_color_t char_color , oled_color_t bg_color, FONTCFG_Typedef *font);
void GLCD_PutHex(uint8_t hexnum , uint8_t xpos, uint8_t ypos, oled_color_t char_color , oled_color_t bg_color, FONTCFG_Typedef *font);
void GLCD_PutHex16(uint16_t hexnum , uint8_t xpos, uint8_t ypos, oled_color_t char_color , oled_color_t bg_color, FONTCFG_Typedef *font);
void GLCD_PutHex32(uint32_t hexnum , uint8_t xpos, uint8_t ypos, oled_color_t char_color , oled_color_t bg_color, FONTCFG_Typedef *font);


#endif	/* SSD1306_H */

