/*
 * lcd_hd44780.h
 *
 *  Created on: Dec 12, 2018
 *      Author: dsolano
 */

#ifndef INC_LCD_HD44780_H_
#define INC_LCD_HD44780_H_



/**
 * @defgroup TM_HD44780
 * @brief    HD44780 LCD driver library for STM32Fxxx - http://stm32f4-discovery.net/2015/07/hal-library-15-hd44780-for-stm32fxxx/
 * @{
 *
 *	It also supports all HD44780 compatible LCD drivers.
 *
 * \par Default pinout
 *
\verbatim
LCD   STM32Fxxx         DESCRIPTION

GND   GND               Ground
VCC   +5V               Power supply for LCD
V0    Potentiometer	    Contrast voltage. Connect to potentiometer
RS    PB2               Register select, can be overwritten in your project's defines.h file
RW    GND               Read/write
E     PB7               Enable pin, can be overwritten in your project's defines.h file
D0    -                 Data 0 - doesn't care
D1    -                 Data 1 - doesn't care
D2    -                 Data 2 - doesn't care
D3    -                 Data 3 - doesn't  care
D4    PC12              Data 4, can be overwritten in your project's defines.h file
D5    PC13              Data 5, can be overwritten in your project's defines.h file
D6    PB12              Data 6, can be overwritten in your project's defines.h file
D7    PB13              Data 7, can be overwritten in your project's defines.h file
A     +3V3              Back light positive power
K     GND               Ground for back light
\endverbatim
 *
 *
 * \par Changelog
 *
\verbatim
 Version 1.0
  - First release
\endverbatim
 *
 * \par Dependencies
 *
\verbatim
 - STM32Fxxx HAL
 - defines.h
 - TM DELAY
 - TM GPIO
\endverbatim
 */
#include "chip.h"
#include "defines.h"
#include <lpc_types.h>
#include <stdint.h>
/*
 * Define interface mode LCD HD44780
 */

#define LCD_HD44780_PARALLEL		0	// Power + 8 Lines - Fastest
#define LCD_HD44780_I2C				1	// Power + 2 wires - Slower
#define LCD_HD44780_I2C_MCP23008	0	// Use this for my own interface plus the I2C
//#define LCD_HD44780_SERIAL		0	// Power + 3 Wires - Fast	//To be implemented in a near future

#if LCD_HD44780_I2C
#define LCD_HD44780_I2C_ADDR                0x27
#endif
#if  LCD_HD44780_I2C_MCP23008
#define LCD_HD44780_I2C_ADDR                0x20
#endif
/**
 * @}
 */

/**
 * @defgroup TM_HD44780_Functions
 * @brief    Library Functions
 * @{
 */

/**
 * @brief  Initializes HD44780 LCD
 * @brief  cols: Width of lcd
 * @param  rows: Height of lcd
 * @retval None
 */
#if LCD_HD44780_I2C || LCD_HD44780_I2C_MCP23008
void lcd_HD44780_Init(uint8_t cols, uint8_t rows, I2C_ID_T i2c, uint8_t i2caddr);
#endif
#if LCD_HD44780_PARALLEL
void lcd_HD44780_Init(uint8_t cols, uint8_t rows);
#endif
/**
 * @brief  Turn display on
 * @param  None
 * @retval None
 */
void lcd_HD44780_DisplayOn(void);

/**
 * @brief  Turn display off
 * @param  None
 * @retval None
 */
void lcd_HD44780_DisplayOff(void);

/**
 * @brief  Clears entire LCD
 * @param  None
 * @retval None
 */
void lcd_HD44780_Clear(void);

/**
 * @brief  Puts string on lcd
 * @param  x: X location where string will start
 * @param  y; Y location where string will start
 * @param  *str: pointer to string to display
 * @retval None
 */
void lcd_HD44780_Puts(uint8_t x, uint8_t y, char* str);


/**
 * @brief  Puts Character on lcd
 * @param  x: X location where string will start
 * @param  y; Y location where string will start
 * @param  c: actual char to display
 * @retval None
 */
void lcd_HD44780_putChar(uint8_t x, uint8_t y, char c);

/**
 * @brief  Enables cursor blink
 * @param  None
 * @retval None
 */
void lcd_HD44780_BlinkOn(void);

/**
 * @brief  Disables cursor blink
 * @param  None
 * @retval None
 */
void lcd_HD44780_BlinkOff(void);

/**
 * @brief  Shows cursor
 * @param  None
 * @retval None
 */
void lcd_HD44780_CursorOn(void);

/**
 * @brief  Hides cursor
 * @param  None
 * @retval None
 */
void lcd_HD44780_CursorOff(void);

/**
 * @brief  Scrolls display to the left
 * @param  None
 * @retval None
 */
void lcd_HD44780_ScrollLeft(uint8_t);

/**
 * @brief  Scrolls display to the right
 * @param  None
 * @retval None
 */
void lcd_HD44780_ScrollRight(uint8_t);

/**
 * @brief  Creates custom character
 * @param  location: Location where to save character on LCD. LCD supports up to 8 custom characters, so locations are 0 - 7
 * @param *data: Pointer to 8-bytes of data for one character
 * @retval None
 */
void lcd_HD44780_CreateChar(uint8_t location, uint8_t* data);

/**
 * @brief  Puts custom created character on LCD
 * @param  x: X location where character will be shown
 * @param  y: Y location where character will be shown
 * @param  location: Location on LCD where character is stored, 0 - 7
 * @retval None
 */
void lcd_HD44780_PutCustom(uint8_t x, uint8_t y, uint8_t location);

/*********************************************************************//**
 * @brief		Puts a decimal number to LCD-I2C
 * @param[in]	decnum	Decimal number (8-bit long)
 * @return		None
 **********************************************************************/
void lcd_HD44780_PutDec99(uint8_t x, uint8_t y, uint8_t decnum);

/*********************************************************************//**
 * @brief		Puts a decimal number to LCD-I2C
 * @param[in]	decnum	Decimal number (8-bit long)
 * @return		None
 **********************************************************************/
void lcd_HD44780_PutDec(uint8_t x, uint8_t y, uint8_t decnum);

/*********************************************************************//**
 * @brief		Puts a decimal number to LCD-I2C
 * @param[in]	decnum	Decimal number (8-bit long)
 * @return		None
 **********************************************************************/
void lcd_HD44780_PutDec16(uint8_t x, uint8_t y, uint16_t decnum);

/*********************************************************************//**
 * @brief		Puts a decimal number to LCD-I2C
 * @param[in]	decnum	Decimal number (8-bit long)
 * @return		None
 **********************************************************************/
void lcd_HD44780_PutDec32(uint8_t x, uint8_t y, uint32_t decnum);

/*********************************************************************//**
 * @brief		Puts a hex number to LCD-I2C port without the 0x chars
 * @param[in]	hexnum	Hex number (8-bit long)
 * @return		None
 **********************************************************************/
void lcd_HD44780_PutHex_(uint8_t x, uint8_t y, uint8_t hexnum);

/*********************************************************************//**
 * @brief		Puts a hex number to LCD-I2C
 * @param[in]	hexnum	Hex number (8-bit long)
 * @return		None
 **********************************************************************/
void lcd_HD44780_PutHex(uint8_t x, uint8_t y, uint8_t hexnum);

/*********************************************************************//**
 * @brief		Puts a hex number to LCD-I2C
 * @param[in]	hexnum	Hex number (16-bit long)
 * @return		None
 **********************************************************************/
void lcd_HD44780_PutHex16(uint8_t x, uint8_t y, uint16_t hexnum);

/*********************************************************************//**
 * @brief		Puts a hex number to LCD-I2C
 * @param[in]	hexnum	Hex number (32-bit long)
 * @return		None
 **********************************************************************/
void lcd_HD44780_PutHex32(uint8_t x, uint8_t y, uint32_t hexnum);



#endif /* INC_LCD_HD44780_H_ */
