/*
 * open1788_bsp.h
 *
 *  Created on: Jul 3, 2018
 *      Author: dsolano
 */

#ifndef INC_GFX_OPEN1788_BSP_H_
#define INC_GFX_OPEN1788_BSP_H_

#include <chip.h>
#include <lpc_types.h>
#include <stdint.h>
/**
 * @brief A struct for Bitmap on LCD screen
 */
typedef struct _Bmp_t
{
  uint32_t  H_Size;
  uint32_t  V_Size;
  uint32_t  BitsPP;
  uint32_t  BytesPP;
  uint32_t *pPalette;
  unsigned long *pPicStream;
  uint8_t *pPicDesc;
} Bmp_t, *pBmp_t;

/**
 * @brief A struct for Font Type on LCD screen
 */

typedef struct _FontType_t
{
  uint32_t H_Size;
  uint32_t V_Size;
  uint32_t CharacterOffset;
  uint32_t CharactersNumber;
  uint8_t *pFontStream;
  uint8_t *pFontDesc;
} FontType_t, *pFontType_t;


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


typedef struct
{
	uint32_t fgcolor;
	uint32_t bgcolor;
	byte orient;
	byte display_model;
	long disp_y_size , disp_x_size;
}LCD_DISPLAY_t;


typedef uint32_t LdcPixel_t, *pLdcPixel_t;

#define MODE_888


#ifdef MODE_565

    /* LCD color */
    #define White          0xFFFF
    #define Black          0x0000
    #define Blue           0xF800
    #define Red            0x001F
    #define Magenta        0xF81F
    #define Green          0x07E0
    #define Yellow         0x07FF


#elif defined MODE_888	 //
    /* LCD color */
    #define White          0xFFFFFF
    #define Black          0x000000
    #define Blue           0xFF0000
    #define Red            0x0000FF
    #define Green          0x00FF00
    #define Cyan	       0xFFFF00
    #define purple         0xff00ff
	#define Yellow		   0x00FFFF
	#define silver		   0xC0C0C0
	#define grey           0x808080

#endif

#define SDRAM_BASE				EMC_ADDRESS_DYCS0
#define LCD_VRAM_BASE_ADDR 		((unsigned long)SDRAM_BASE + 0x00010000)
#define LCD_CURSOR_BASE_ADDR 	((unsigned long)0x20000000)//0x20088800)
#define LEFT 0
#define RIGHT 9999
#define CENTER 9998

#define C_GLCD_REFRESH_FREQ     (50)	// Hz
#define C_GLCD_H_SIZE           480
#define C_GLCD_H_PULSE          2	//
#define C_GLCD_H_FRONT_PORCH    5	//
#define C_GLCD_H_BACK_PORCH     40 	//
#define C_GLCD_V_SIZE           272
#define C_GLCD_V_PULSE          2
#define C_GLCD_V_FRONT_PORCH    8
#define C_GLCD_V_BACK_PORCH     8

#define C_GLCD_PWR_ENA_DIS_DLY  10000
#define C_GLCD_ENA_DIS_DLY      10000

#define CRSR_PIX_32     0
#define CRSR_PIX_64     1
#define CRSR_ASYNC      0
#define CRSR_FRAME_SYNC 2

#define TEXT_DEF_TAB_SIZE 5

#define TEXT_BEL1_FUNC()

void open1788_LCD_SetPallet (const unsigned long * pPallete);
void open1788_LCDinit(void);
void open1788_LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color);
void open1788_LCD_SetPoint(uint16_t Xpos, uint16_t Ypos, uint32_t color);
void open1788_LCD_Clear(uint32_t color);
void open1788_LCD_display_coord(uint16_t x, uint16_t y, uint32_t color, uint32_t color1);

void open1788_LCD_printChar(byte c, int x, int y);
void open1788_LCD_rotateChar(byte c, int x, int y, int pos, int deg);
void open1788_LCD_print(unsigned char *st, int x, int y, int deg);
void open1788_LCD_printNumI(long num, int x, int y);
void open1788_LCD_printNumF(double num, byte dec, int x, int y);

void open1788_LCD_setFont(const uint8_t* font, const uint8_t* font_param);
void open1788_LCD_setColor(uint32_t color);
void open1788_LCD_setBackColor(uint32_t color);
int open1788_LCD_getDisplayXSize(void);
int open1788_LCD_getDisplayYSize(void);
uint8_t* open1788_LCD_getFont(void);
uint8_t* open1788_LCD_getFont_param(void);
uint8_t open1788_LCD_getFontXsize(void);
uint8_t open1788_LCD_getFontYsize(void);
uint16_t open1788_LCD_getColor(void);
uint16_t open1788_LCD_getBackColor(void);
void open1788_LCD_drawBitmap_(int x, int y, int sx, int sy, uint16_t* data, int scale);
void open1788_LCD_drawBitmap(int x, int y, int sx, int sy, uint16_t* data, int deg, int rox, int roy);
void open1788_LCD_drawHLine(int x, int y, int l);
void open1788_LCD_drawVLine(int x, int y, int l);
void open1788_LCD_drawRect(int x1, int y1, int x2, int y2);
void open1788_LCD_drawRoundRect(int x1, int y1, int x2, int y2);
void open1788_LCD_fillRect(int x1, int y1, int x2, int y2);
void open1788_LCD_fillRoundRect(int x1, int y1, int x2, int y2);


#endif /* INC_GFX_OPEN1788_BSP_H_ */
