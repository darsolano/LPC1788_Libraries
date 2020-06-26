/*
 * max7219.c
 *
 *  Created on: Apr 10, 2019
 *      Author: dsolano
 */


#include <max7219.h>
#include <hardware_delay.h>
#include <define_pins.h>
#include <string.h>

MAX7219_s max;

/*
 * Defined method to send data to the serial interface
 */
#define MAX7219(reg,data)	max7219_serial3wire_write_short(((reg<<8)&CMD_MASK) | (data&DATA_MASK))
#define LEDCHAR_OFFSET	32

DEFINE_PIN(shift_CLK,1,30 )
DEFINE_PIN(shift_CS,1,31)
DEFINE_PIN(shift_DOUT,2,12)

uint8_t matrix[64];


/*
 * Segments to be switched on for characters and digits on
 * 7-Segment Displays
 */
const static char charTable[128] = {
    0b01111110,//0
	0b00110000,//1
	0b01101101,//2
	0b01111001,//3
	0b00110011,//4
	0b01011011,//5
	0b01011111,//6
	0b01110000,//7
    0b01111111,//8
	0b01111011,//9
	0b01110111,//A
	0b00011111,//B
	0b00001101,//C
	0b00111101,//d
	0b01001111,//E
	0b01000111,//F
    0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
    0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
    0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
    0b00000000,0b00000000,0b00000000,0b00000000,0b10000000,0b00000001,0b10000000,0b00000000,
    0b01111110,//0
	0b00110000,//1
	0b01101101,//2
	0b01111001,//3
	0b00110011,//4
	0b01011011,//5
	0b01011111,//6
	0b01110000,//7
    0b01111111,//8
	0b01111011,//9
	0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,
	0b01110111,//A
	0b00011111,//B
	0b00001101,//C
	0b00111101,//D
	0b01001111,//E
	0b01000111,//F
	0b00000000,//G
    0b00110111,//H
	0b00000110,//I
	0b00111000,//J
	0b00000000,//K
	0b00001110,//L
	0b00000000,//M
	0b00000000,//N
	0b00011101,//O
    0b01100111,//P
	0b00000000,//Q
	0b00000101,//R
	0b00000000,//S
	0b00001111,//T
	0b00111110,//U
	0b00000000,//V
	0b00000000,//W
    0b00000000,//X
	0b00111011,//Y
	0b00000000,//Z
	0b00000000,0b00000000,0b00000000,0b00000000,0b00001000,0b00000000,
	0b01110111,//a
	0b00011111,//b
	0b00001101,//c
	0b00111101,//d
	0b01001111,//e
	0b01000111,//f
	0b00000000,//g
	0b00010111,//h
	0b00000100,//i
	0b00111000,//j
	0b00000000,//k
	0b00001110,//l
	0b00000000,//m
	0b00000000,//n
	0b00011101,//o
    0b01100111,//p
	0b00000000,//q
	0b00000101,//r
	0b00000000,//s
	0b00001111,//t
	0b00011100,//u
	0b00000000,//v
	0b00000000,//w
    0b00000000,//x
	0b00111011,//y
	0b00000000,//z
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000
};

//Number definition
const uint8_t display7s[] =
{
//        pABCDEFG
		0b01111110,				// 0
		0b00110000,             // 1
		0b01101101,             // 2
		0b01111001,             // 3
		0b00110011,             // 4
		0b01011011,             // 5
		0b01011111,             // 6
		0b01110000,             // 7
		0b01111111,             // 8
		0b01110011,             // 9
		0b01110111,             // A
		0b00011111,             // B
		0b01001110,             // C
		0b00111101,             // D
		0b01001111,             // E
		0b01000111,             // F
		0b10000000				// Dot
//        pABCDEFG
		};

/*
 *        a
        ------
 *      |    |b
 *     f| g  |
 *      ------
 *      |    |c
 *    e |    |
 *      ------ o h
 *         d
 */
const uint8_t display7sLetters[] =
{
//        pABCDEFG
		0b00000000,				// space or blank
		0b01110111,             // A
		0b00011111,             // B
		0b01001110,             // C
		0b00111101,             // D
		0b01001111,             // E
		0b01000111,             // F
		0b01111011,				// G
		0b00110111,				// H
		0b00000110,				// I
		0b00111100,				// J
		0b00000001,				// K
		0b00001110,				// L
		0b00000001,				// M
		0b00010101,				// N
		0b00011101,				// O
		0b01100111,				// P
		0b11111110,				// Q
		0b00000101,				// R
		0b01011011,				// S
		0b00001111,				// T
		0b00111110,				// U
		0b00011100,				// V
		0b00000001,				// W
		0b00000001,				// X
		0b00111011,				// Y
		0b00000001				// Z
//		  pABCDEFG
		};



//SPI BitBang to send data 16 bits
static void max7219_serial3wire_write_short(uint16_t data)
{
	uint16_t temp , count;

	temp=data;
	shift_CLK_LOW();
	for( count=0; count<16; count++)
	{
		if(temp & 0x8000)
			shift_DOUT_HIGH();
		else
			shift_DOUT_LOW();
		temp <<=  1;
		// pseudo clock
		shift_CLK_HIGH();
		__NOP();
		shift_CLK_LOW();
		__NOP();
	}
}


static uint8_t get_ascii(uint8_t raw){
	return charTable[raw];
}


/*
 * Send data to the specific device in the chain
 * the device data must be preceded and followed by NOP´s
 * depending on the location of the specific device in the chain
 * Devices must start from 0 to xx, max devices is from 1 to max
 */
static void max7219_Send_Data(uint8_t reg, uint8_t data, uint8_t device){
	int preced_nop, follow_nop = 0;

	if (device > max.max_dev_number) {
		return;
	}

	// Determine how many NOP´s before and after should be sent
	preced_nop = max.max_dev_number - (device + 1);
	follow_nop = max.max_dev_number - (preced_nop + 1);

	shift_CS_SELECTED();

	for (int i = 0; i < preced_nop; i++) {
		MAX7219(NOP,0);
	}

	MAX7219(reg,data);

	for (int i = 0; i < follow_nop; i++) {
		MAX7219(NOP,0);
	}
	shift_CS_DESELECTED();
}

void max7219_init(SCAN_LIMIT_e digits , INTENSISTY_e intensity , DECODE_e decode, uint8_t max_dev)
{
	Chip_IOCON_Init(LPC_IOCON);
	Chip_GPIO_Init(LPC_GPIO);
	// Set DIRECTION for Pins
	shift_CS_OUTPUT();
	shift_CLK_OUTPUT();
	shift_DOUT_OUTPUT();
	// Initial state
	shift_CS_DESELECTED();
	shift_CLK_LOW();
	shift_DOUT_LOW();

	max.scan_limit = digits;
	max.intensity = intensity;
	max.decode_mode = decode;
	max.max_dev_number = max_dev;
	max.disp_test = DISP_TEST_NORMAL_OP;
	max.shutdown_mode = NORMAL_OPERATION;

	for (int i = 0; i < max.max_dev_number; i++) {
		max7219_Send_Data(DISPLAY_TEST, max.disp_test, i);
		max7219_Send_Data(SHUTDOWN, max.shutdown_mode, i);
		max7219_Send_Data(SCAN_LIMIT, max.scan_limit, i);
		max7219_Send_Data(INTENSITY, max.intensity, i);
		max7219_Send_Data(DECODE_MODE, max.decode_mode, i);


		//max7219_test_cycle(i);

		_delay_ms(500);

		max7219_clear(i);
	}
}

void max7219_test_cycle(uint8_t dev)
{
	max7219_Send_Data(DISPLAY_TEST, DISP_TEST_TEST_MODE, dev);
	_delay_ms(250);
	max7219_Send_Data(DISPLAY_TEST , DISP_TEST_NORMAL_OP, dev);

	// circle around display
	int i = 0;
	uint8_t digit = 0;

	// round display from a to dot
	for (i=0;i<9;i++)
	{
		//max7219_serial3wire_write_short((digit<<8)|digit);
		max7219_serial3wire_write_short((digit<<8)|digit);
		max7219_Send_Data(DIG0,digit, dev);
		max7219_Send_Data(DIG1,digit, dev);
		max7219_Send_Data(DIG2,digit, dev);
		max7219_Send_Data(DIG3,digit, dev);
		max7219_Send_Data(DIG4,digit, dev);
		max7219_Send_Data(DIG5,digit, dev);
		max7219_Send_Data(DIG6,digit, dev);
		max7219_Send_Data(DIG7,digit, dev);
		digit = (1<<i);
		_delay_ms(250);
	}

	max7219_blink(dev, 250,3);

	//Count to 0-9 for each display from display 0 to 7

	// count from 0-9 every display together
}

void max7219_blink(uint8_t dev, int timeinmilli, int blink_times)
{
	max7219_Send_Data(DIG0,0xff, dev);
	max7219_Send_Data(DIG1,0xff, dev);
	max7219_Send_Data(DIG2,0xff, dev);
	max7219_Send_Data(DIG3,0xff, dev);
	max7219_Send_Data(DIG4,0xff, dev);
	max7219_Send_Data(DIG5,0xff, dev);
	max7219_Send_Data(DIG6,0xff, dev);
	max7219_Send_Data(DIG7,0xff, dev);
	while (blink_times--)
	{
		max7219_Send_Data(SHUTDOWN , SHUTDOWN_MODE, dev);
		_delay_ms(timeinmilli);
		max7219_Send_Data(SHUTDOWN , NORMAL_OPERATION, dev);
		_delay_ms(timeinmilli);
	}
}

void max7219_clear(uint8_t dev)
{
	int i = 9;
	while (i--)	// i = digit register. BUT 0 IS NOT A DIGIT REGISTER, Start from 1 thru 8
	{
		max7219_Send_Data(i, (CLEAR_MASK&DATA_MASK), dev);
	}

}


void max7219_shutdown(uint8_t dev, Bool mode){
	if (mode){
		max7219_Send_Data(SHUTDOWN , SHUTDOWN_MODE, dev);
	}else{
		max7219_Send_Data(SHUTDOWN , NORMAL_OPERATION, dev);
	}
}

void max7219_clear_digit(uint8_t dev, uint8_t digit){
	max7219_Send_Data(digit , 0, dev);
}

void max7219_blink_display(uint8_t dev, int timeinmilli, int blink_times){

	while (blink_times--)
	{
		max7219_Send_Data(SHUTDOWN , SHUTDOWN_MODE, dev);
		_delay_ms(timeinmilli);
		max7219_Send_Data(SHUTDOWN , NORMAL_OPERATION, dev);
		_delay_ms(timeinmilli);
	}
}


void max7219_disp_digit(uint8_t dev, uint8_t digit, uint8_t value, Bool dp) {
    int offset;
    uint8_t v;

    if(dev<0 || dev>=max.max_dev_number) return;
    if(digit<0 || digit>7 || value>15) return;
    offset=dev*8;
    v=charTable[value];
    if(dp) v|=0b10000000;
    matrix[offset+digit]=v;
    max7219_Send_Data(digit+1,v, dev);
}

void max7219_disp_char(uint8_t dev, uint8_t digit, char value, Bool dp) {
    int offset;
    uint8_t index,v;

    if(dev<0 || dev>=max.max_dev_number) return;
    if(digit<0 || digit>7) return;
    offset=dev*8;
    index=(uint8_t)value;
    if(index >127) {
    	//nothing define we use the space char
    	value=32;
    }
    v=charTable[index];
    if(dp) v|=0b10000000;
    matrix[offset+digit]=v;
    max7219_Send_Data(digit+1,v,dev);
}

void max7219_disp_string(uint8_t dev, uint8_t pos, char* str){

	if(dev<0 || dev>=max.max_dev_number) return;
    if(pos<0 || pos>7) return;
    if (strlen(str) >4) return;

    while (*str){
		max7219_disp_char(dev, pos--, *str++, 0);
	}
}

void max7219_disp_raw(uint8_t dev, uint8_t pos, uint8_t value){
	max7219_Send_Data(pos+1 , value, dev);
}

/************************************END OF***************************/
void max7219_print_32(uint8_t dev, uint32_t number)
{
	// Digit 0
	uint8_t c1 = number % 10;
	max7219_Send_Data(DIG0 , get_ascii(c1), dev);

	// Digit 1
	uint8_t c2 = (number / 10) % 10;
	max7219_Send_Data(DIG1 , get_ascii(c2), dev);

	// Digit 2
	uint8_t c3 = (number / 100) % 10;
	max7219_Send_Data(DIG2 , get_ascii(c3), dev);

	// Digit 3
	uint8_t c4 = (number / 1000) % 10;
	max7219_Send_Data(DIG3 , get_ascii(c4), dev);

	// Digit 4
	uint8_t c5 = (number / 10000) % 10;
	max7219_Send_Data(DIG4 , get_ascii(c5), dev);

	// Digit 5
	uint8_t c6 = (number / 100000) % 10;
	max7219_Send_Data(DIG5 , get_ascii(c6), dev);

	// Digit 6
	uint8_t c7 = (number / 1000000) % 10;
	max7219_Send_Data(DIG6 , get_ascii(c7), dev);

	// Digit 7
	uint8_t c8 = (number / 10000000) % 10;
	max7219_Send_Data(DIG7 , get_ascii(c8), dev);
}


void max7219_print_16(uint8_t dev, uint16_t number)
{
	// Digit 0
	uint8_t c1 = number % 10;
	max7219_Send_Data(DIG0 , get_ascii(c1), dev);

	// Digit 1
	uint8_t c2 = (number / 10) % 10;
	max7219_Send_Data(DIG1 , get_ascii(c2), dev);

	// Digit 2
	uint8_t c3 = (number / 100) % 10;
	max7219_Send_Data(DIG2 , get_ascii(c3), dev);

	// Digit 3
	uint8_t c4 = (number / 1000) % 10;
	max7219_Send_Data(DIG3 , get_ascii(c4), dev);

	// Digit 4
	uint8_t c5 = (number / 10000) % 10;
	max7219_Send_Data(DIG4 , get_ascii(c5), dev);

}

void max7219_print_Dec2digit(uint8_t dev, uint8_t pos, uint8_t number, Bool dp)
{
	uint8_t digit_pos = pos + 1;

	// Digit 0
	uint8_t c1 = number % 10;
	// Digit 1
	uint8_t c2 = (number / 10) % 10;
	max7219_Send_Data(digit_pos+1 , get_ascii(c2), dev);
	max7219_Send_Data(digit_pos   , dp ? get_ascii(c1)|0x80:get_ascii(c1), dev);
}


void max7219_print_8(uint8_t dev, uint8_t pos, uint8_t number)
{
	uint8_t digit_pos = pos + 1;

	// Digit 0
	uint8_t c1 = number % 10;
	// Digit 1
	uint8_t c2 = (number / 10) % 10;
	// Digit 2
	uint8_t c3 = (number / 100) % 10;
	if ( c3 != 0)
		max7219_Send_Data(digit_pos+2 , get_ascii(c3), dev);
	if (c3 == 0 && c2 != 0)
		max7219_Send_Data(digit_pos+1 , get_ascii(c2), dev);

	max7219_Send_Data(digit_pos , get_ascii(c1), dev);
}


void max7219_print_hex_8(uint8_t dev, uint8_t hexnum)
{
	uint8_t nibble, i;

	i = 1;
	do
	{
		nibble = (hexnum >> (4 * i)) & 0x0F;
		max7219_Send_Data(i+1, get_ascii(nibble), dev);
	} while (i--);
}

void max7219_print_hex_16(uint8_t dev, uint16_t hexnum)
{
	uint8_t nibble, i;

	i = 3;
	do
	{
		nibble = (hexnum >> (4 * i)) & 0x0F;
		max7219_Send_Data(i+1, get_ascii(nibble), dev);
	} while (i--);
}

void max7219_print_hex_32(uint8_t dev, uint32_t hexnum)
{
	uint8_t nibble, i;

	i = 7;
	do
	{
		nibble = (hexnum >> (4 * i)) & 0x0F;
		max7219_Send_Data(i+1, get_ascii(nibble), dev);
	} while (i--);
}

/*
 * *******************************************
 * LED MATRIX 8X8 FUNCTIONS
 * *******************************************
 */
void max7219_ledmtrx_setcol(uint8_t dev, uint8_t col, uint8_t value){
    uint8_t val;

    if(dev<0 || dev>=max.max_dev_number) return;
    if(col<0 || col>7) return;
    for(int row=0;row<8;row++) {
    	val=value >> (7-row);
    	val=val & 0x01;
    	max7219_ledmtrx_setled(dev, row, col, val);
    }

}

void max7219_ledmtrx_setrow(uint8_t dev, uint8_t row, uint8_t value){
    int offset;
    if(dev<0 || dev>=max.max_dev_number) return;
    if(row<0 || row>7) return;
    offset=dev*8;
    matrix[offset+row]=value;

    max7219_Send_Data(row+1, matrix[offset+row], dev);
}

void max7219_ledmtrx_setled(uint8_t dev, uint8_t row, uint8_t col, Bool state){
	int offset;
	uint8_t val=0x00;

	if(dev<0 || dev>=max.max_dev_number) return;
	if(row<0 || row>7 || col<0 || col>7) return;
	offset=dev*8;
	val=0b10000000 >> col;
	if(state)
		matrix[offset+row] = matrix[offset+row]|val;
	else {
		val=~val;
		matrix[offset+row]=matrix[offset+row]&val;
	}

	max7219_Send_Data(row+1, matrix[offset+row], dev);
}

void max7219_ledmtrx_printchar(char chr, uint8_t dev){
	uint8_t row;

	for (row=1;row<6;row++){
		max7219_ledmtrx_setrow(dev, row, chr);
	}
}
