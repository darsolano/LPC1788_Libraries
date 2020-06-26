/*
 * max7219.h
 *
 *  Created on: Apr 10, 2019
 *      Author: dsolano
 */

#ifndef INCLUDE_MAX7219_H_
#define INCLUDE_MAX7219_H_

#include <chip.h>
#include <lpc_types.h>

#define CMD_MASK	0x0f00
#define DATA_MASK	0x00ff
#define CLEAR_MASK	0x00

// Characters		  pABCDEFG
#define DASH	  	0b00000001
#define DOT			0b10000000
#define DEGREE		0b01100011

typedef enum
{
	NO_SCROLL = 0,
	SCROLL_LEFT = 1,
	SCROLL_RIGHT = 2
}SCROLL_DIR_e;

typedef enum
{
	LETTER,
	NUMBER
}TYPEOFCHAR_t;

typedef enum
{
	NOP = 0x00,
	DIG0 = 0x01,
	DIG1 = 0x02,
	DIG2 = 0x03,
	DIG3 = 0x04,
	DIG4 = 0x05,
	DIG5 = 0x06,
	DIG6 = 0x07,
	DIG7 = 0x08,
	DECODE_MODE = 	0x09,
	INTENSITY 	= 	0x0A,
	SCAN_LIMIT 	= 	0x0B,
	SHUTDOWN 	= 	0x0C,
	DISPLAY_TEST = 0x0F
}MAX7219_REGS_t;

typedef enum
{
	INT_MIN = 0,
	INT_3 = 1,
	INT_5 = 2,
	INT_7 = 3,
	INT_9 = 4,
	INT_11 = 5,
	INT_13 = 6,
	INT_15 = 7,
	INT_MED = 8,
	INT_19 = 9,
	INT_21 = 10,
	INT_23 = 11,
	INT_25 = 12,
	INT_27 = 13,
	INT_29 = 14,
	INT_MAX = 15
}INTENSISTY_e;

// Shutdown parameter
typedef enum
{
	SHUTDOWN_MODE =		0x00,
	NORMAL_OPERATION =	0x01
}SHUTDOWN_e;


// DEcode register parameters
typedef enum
{
	NO_DECODE_DIG_0_7 =	0x00,
	CODE_B_DIG_0 =		0x01,
	CODE_B_DIG_0_3 =	0x0F,
	CODE_B_ALL_DIG =	0xFF
}DECODE_e;

// Test register parameter
typedef enum
{
	DISP_TEST_NORMAL_OP =	0x00,
	DISP_TEST_TEST_MODE =	0x01
}DISP_TEST_e;

// Determine how many digits to use are connected to MAX7219
typedef enum
{
	DISP_ONLY_DIG0 = 0,
	DISP_DIG0_DIG1 = 1,
	DISP_DIG0_to_DIG2 = 2,
	DISP_DIG0_to_DIG3 = 3,
	DISP_DIG0_to_DIG4 = 4,
	DISP_DIG0_to_DIG5 = 5,
	DISP_DIG0_to_DIG6 = 6,
	DISP_ALL_DIGITS = 7
}SCAN_LIMIT_e;

typedef struct
{
	uint8_t 		no_op_mode;
	uint8_t 		decode_mode;
	uint8_t			max_dev_number;
	INTENSISTY_e 	intensity;
	SCAN_LIMIT_e 	scan_limit;
	DISP_TEST_e 	disp_test;
	SHUTDOWN_e 		shutdown_mode;
}MAX7219_s;
/*
 * Data structure to be used for MAX7219 operation
 */
extern MAX7219_s max;


/*	Initial configuration for MAX7219, go for no config to use 7Segment array
 * 	Each display declare in the init max devices will be tested and programmed
 * 	in the same way, with the same parameters
 */
void max7219_init(SCAN_LIMIT_e digits , INTENSISTY_e intensity , DECODE_e decode, uint8_t dev_num);


void max7219_scroll_display(uint8_t dev, SCROLL_DIR_e direction);

/*
 * Blink a block or device with in the array
 * dev : device number
 * timeinmilli : millisecond between blinks
 * blink_times : time count to blink
 */
void max7219_blink(uint8_t dev, int timeinmilli, int blink_times);

/*
 * Test a 7 segments display in the array
 * first all segment in a round manner to the dot
 * second all will blink for all digits
 * dev : device in array
 */
void max7219_test_cycle(uint8_t dev);

/*
 * Enters shutdown mode as per request in order to
 * drop down current consumption in the circuit
 * dev = dev number within the array of devices
 * mode true = shutdown mode
 * mode false = Normal mode
 */
void max7219_shutdown(uint8_t dev, Bool mode);

/*
 * Clear all digits in an specific device in the array
 * dev : device in array
 */
void max7219_clear(uint8_t dev);

/*
 * DIgits are counted from right to left in the array
 * device are counted from left to right in the array
 * dev = dev number within the array of devices
 * digit =
 */
void max7219_clear_digit(uint8_t dev, uint8_t digit);

void max7219_blink_display(uint8_t dev, int timeinmilli, int blink_times);
/*
 * Print a digit character in a digit position within a device in the array to displays
 * from digit 8 to digit 1, from left to right
 * 7 segment array use only
 */
void max7219_disp_digit(uint8_t dev, uint8_t digit, uint8_t value, Bool dp);

/*********************************************************************//**
 * @brief		Display a single char to 7 segment
 * @param[in]	dev	device number in array
 * @param[in]	digit is the position within the device
 * @param[in]	dp boolean variable to indicate if the char has an active dot
 * @return		None
 **********************************************************************/
void max7219_disp_char(uint8_t dev, uint8_t digit, char value, Bool dp);

/*********************************************************************//**
 * @brief		Display a string of char to 7 segment
 * @param[in]	dev	device number in array
 * @param[in]	from pos is the position within the device to start printing chars
 * @param[in]	dp boolean variable to indicate if the char has an active dot
 * @return		None
 **********************************************************************/
void max7219_disp_string(uint8_t dev, uint8_t pos, char* str);

/*********************************************************************//**
 * @brief		Display a raw pattern to 7 segment
 * @param[in]	dev	device number in array
 * @param[in]	from pos is the position within the device to start printing chars
 * @param[in]	value is the custom pattern to be display in 7 segment
 * @return		None
 **********************************************************************/
void max7219_disp_raw(uint8_t dev, uint8_t pos, uint8_t value);

/*********************************************************************//**
 * @brief		Puts a DEC number to 7 segment
 * @param[in]	dev	device number in array
 * @param[in]	hexnum	Hex number (8-bit long)
 * @return		None
 **********************************************************************/
void max7219_print_Dec2digit(uint8_t dev, uint8_t pos, uint8_t number, Bool dp);
void max7219_print_8(uint8_t dev, uint8_t pos, uint8_t number);
void max7219_print_16(uint8_t dev, uint16_t number);
void max7219_print_32(uint8_t dev, uint32_t number);

/*********************************************************************//**
 * @brief		Puts a hex number to 7 segment
 * @param[in]	dev	device number in array
 * @param[in]	hexnum	Hex number (8-bit long)
 * @return		None
 **********************************************************************/
void max7219_print_hex_8(uint8_t dev, uint8_t hexnum);

/*********************************************************************//**
 * @brief		Puts a hex number to 7 segment
  * @param[in]	dev	device number in array
 * @param[in]	hexnum	Hex number (16-bit long)
 * @return		None
 **********************************************************************/
void max7219_print_hex_16(uint8_t dev, uint16_t hexnum);

/*********************************************************************//**
 * @brief		Puts a hex number to 7 segment
  * @param[in]	dev	device number in array
 * @param[in]	hexnum	Hex number (32-bit long)
 * @return		None
 **********************************************************************/
void max7219_print_hex_32(uint8_t dev, uint32_t hexnum);

void max7219_ledmtrx_setcol(uint8_t dev, uint8_t col, uint8_t value);
void max7219_ledmtrx_setrow(uint8_t dev, uint8_t row, uint8_t value);
void max7219_ledmtrx_setled(uint8_t dev, uint8_t row, uint8_t col, Bool state);
void max7219_ledmtrx_printchar(char chr, uint8_t dev);

#endif /* INCLUDE_MAX7219_H_ */
