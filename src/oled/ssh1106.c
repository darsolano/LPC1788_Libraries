/*
 * ssh1106.c
 *
 *  Created on: Jan 29, 2019
 *      Author: dsolano
 */

#include <oled/ssh1106.h>
#include <hardware_delay.h>
#include <define_pins.h>
#include <oled/defaultfont.h>

DEFINE_PIN(SSH1106_CS, 1, 21)	// SS Pin on SSP1
DEFINE_PIN(SSH1106_D_C, 1, 30)	// D = Active HIGH; C = Active LOW
DEFINE_PIN(SSH1106_RST, 1, 31)	// ON or OFF

static void ssh1106_setup_ram(void);
static void ssh1106_write_pixel(int x, int y, ssh1106_color_t color);
static void ssh1106_reset(void);

uint8_t ssh1106_spiIO(uint8_t data) {
	while (!(SSH1106SSPx->SR & SSP_STAT_TFE))
		;
	SSH1106SSPx->DR = data;		// send a byte
	while (!(SSH1106SSPx->SR & SSP_STAT_RNE))
		;
	return SSH1106SSPx->DR;		// Receive a byte
}

/*
 Indicate to LCD controller that SPI transmissions are commands
 */
void ssh1106_SetCommandMode(void) {
	SSH1106_D_C_LOW();
}

/*
 Indicate to LCD controller that SPI transmissions are addressed to display
 */
void ssh1106_SetDataMode(void) {
	SSH1106_D_C_HIGH();
}

/*
 Set the column address for subsequent display writes
 */
void ssh1106_SetColumnStart(int x) {
	if (x >= 0 && x < 132) {
		ssh1106_SetCommandMode();
		SSH1106_CS_SELECTED();
		ssh1106_spiIO(x & 0x0F); // splits into lower and higher 4 nibbles
		ssh1106_spiIO(((x & 0xF0) >> 4) + 0x10);
		SSH1106_CS_DESELECTED();
	}
}

/*
 Set the RAM page for subsequent display writes
 There are 8 pages of height 8
 */
void ssh1106_SetPageStart(int x) {
	if (x >= 0 && x < 8) {
		ssh1106_SetCommandMode();
		SSH1106_CS_SELECTED();
		ssh1106_spiIO(x + 0xB0);
		SSH1106_CS_DESELECTED();
	}
}

static void ssh1106_reset(void){
	SSH1106_RST_HIGH(); // power cycle LCD
	_delay_uS(5);
	SSH1106_RST_LOW();
	_delay_uS(5);
	SSH1106_RST_HIGH(); // power cycle LCD
	_delay_uS(5);
}
/*
 Set appropriate pins as outputs
 */
static void ssh1106_InitPins(void) {
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

	SSH1106_CS_OUTPUT();
	SSH1106_D_C_OUTPUT();
	SSH1106_RST_OUTPUT();
	Chip_SSP_Init(SSH1106SSPx);
	Chip_SSP_SetMaster(SSH1106SSPx, TRUE);
	Chip_SSP_SetBitRate(SSH1106SSPx, 1000000);
	Chip_SSP_Enable(SSH1106SSPx);
	SSH1106_CS_DESELECTED();

}

/*
 Initialize LCD display
 */
void ssh1106_Init(void) {
	ssh1106_InitPins();
	ssh1106_reset();
	ssh1106_SetCommandMode();
	SSH1106_CS_SELECTED();
	ssh1106_spiIO(0xAF); // turn on LCD
	SSH1106_CS_DESELECTED();
	_delay_ms(100);
}

/*
 Set up LCD for writes beginning at upper-left corner of display
 */
static void ssh1106_setup_ram(void) {
	ssh1106_SetCommandMode();
	SSH1106_CS_SELECTED();
	ssh1106_spiIO(0x02);
	ssh1106_spiIO(0x10); // set RAM column start to 2, accounting for display's 132 pixel
	ssh1106_spiIO(0xB0); // set page to 0
	SSH1106_CS_DESELECTED();
}

/*
 Fills screen with either white or black pixels
 */
void ssh1106_clear_screen(ssh1106_color_t color) {
	unsigned char fill = (color) ? 0xFF : 0x00; // fill screen with either white (on) or black (off)
	ssh1106_SetCommandMode(); // set column to 0
	SSH1106_CS_SELECTED();
	ssh1106_spiIO(0x00);
	ssh1106_spiIO(0x10);

	for (int x = 0; x < 8; x++) {
		ssh1106_spiIO(0xB0 + x);
		ssh1106_SetDataMode();
		for (int i = 0; i < 132; i++)
			ssh1106_spiIO(fill);
		ssh1106_SetCommandMode();
	}
	SSH1106_CS_DESELECTED();
}

/*
 Writes a single pixel to the LCD.
 Note that this will also set all pixels in the corresponding byte
 */
static void ssh1106_write_pixel(int x, int y, ssh1106_color_t color) {
	unsigned char fill = (color) ? 0xFF : 0x00; // default is dark pixel. These bits are all flipped later.

	if (x >= 0 && x < 128 && y >= 0 && y / 8 < 8) { // check that coordinates are in bound, and that character exists
		ssh1106_SetCommandMode();

		ssh1106_SetColumnStart(x + 0x2); // RAM column offset of 2 since 1106 has buffer of size 132 centered on screen of width 128
		ssh1106_SetPageStart(y / 8); // set page address Text is aligned with RAM pages

		ssh1106_SetDataMode();
		SSH1106_CS_SELECTED();
		ssh1106_spiIO((~fill) ^ ((0x1) << (y % 8))); // set one bit only
		SSH1106_CS_DESELECTED();
	}
}

/*
 Write a horizontal or vertical line.
 Note that in both cases, any accessed bytes will have all other bits turned off
 */
void ssh1106_DrawLine(int x, int y, int length, bool horizontal, ssh1106_color_t color) {
	unsigned char fill = (color) ? 0xFF : 0x00; // default is dark pixel. These bits are all flipped later.

	ssh1106_SetCommandMode();

	if (horizontal) {
		ssh1106_SetColumnStart(x + 0x02);

		ssh1106_SetPageStart(y / 8); // set page address
		ssh1106_SetDataMode();
		SSH1106_CS_SELECTED();
		for (int i = 0; i < length; i++)
			ssh1106_spiIO((~fill) ^ ((0x1) << (y % 8))); // set appropriate bit
		SSH1106_CS_DESELECTED();
	} else {
		uint16_t startMask = (1 << y % 8) - 1;
		uint16_t endMask = (1 << (y + length) % 8) - 1;

		ssh1106_SetColumnStart(0x2 + x);
		ssh1106_SetPageStart(y / 8); // RAM has 8 pages
		ssh1106_SetDataMode();
		SSH1106_CS_SELECTED();
		ssh1106_spiIO(startMask ^ (fill)); // set appropriate bits for first page
		SSH1106_CS_DESELECTED();

		for (int i = 0; i < (length - (8 - (y % 8))) / 8; i++) { // limited to number of full pages
			ssh1106_SetCommandMode();
			ssh1106_SetColumnStart(0x2 + x);
			ssh1106_SetPageStart(y / 8 + i + 1); // RAM has 8 pages
			ssh1106_SetDataMode();
			SSH1106_CS_SELECTED();
			ssh1106_spiIO(fill);
			SSH1106_CS_DESELECTED();
		}

		ssh1106_SetCommandMode();
		ssh1106_SetColumnStart(0x2 + x);
		ssh1106_SetPageStart((y + length) / 8);
		ssh1106_SetDataMode();

		SSH1106_CS_SELECTED();
		if ((y % 8) + length == 8) // exactly than 1 page
			return;
		else if ((y % 8) + length > 8) // longer than 1 page
			ssh1106_spiIO(endMask ^ (~fill));
		else
			// shorter than one page
			ssh1106_spiIO(endMask ^ startMask ^ (~fill));
		SSH1106_CS_DESELECTED();

		return;
	}
}

/*
 Writes a single character to the specified location. Only ASCII characters supported
 */
void ssh1106_WriteChar(int x, int y, char c) {
	if (x >= 0 && x < 128 - 5 && y >= 0 && y / 8 < 8 && c < 255) { // check that coordinates are in bound, and that character exists
		ssh1106_SetCommandMode();

		ssh1106_SetColumnStart(x + 0x2); // RAM column offset of 2 since 1106 has buffer of size 132 centered on screen of width 128
		ssh1106_SetPageStart(y / 8); // set page address Text is aligned with RAM pages

		ssh1106_SetDataMode();
		SSH1106_CS_SELECTED();
		for (int i = 0; i < 5; i++)
			ssh1106_spiIO(~font[i + c * 5]);
		ssh1106_spiIO(0xFF);
		SSH1106_CS_DESELECTED();

	}
}

/*
 Write a string to the display at the desired location. Uses C Strings
 */
void ssh1106_WriteWord(int x, int y, int length, char* word, bool align) {
	int charStart = x + 2; // tracks starting index of current char
	int pageIndex = y / 8;

	// check that coordinates are in bound, and at least one character can be written at point
	if (x >= 0 && x < 128 - 5 && y >= 0 && y / 8 < 8) {
		ssh1106_SetColumnStart(x + 0x2); // RAM column offset of 2 since 1106 has buffer of size 132 centered on screen of width 128
		ssh1106_SetPageStart(y / 8); // set page address

		ssh1106_SetDataMode();
		for (int j = 0; j < length; j++) {
			if (charStart > 130 - 5) { // check if there is room for next char
				ssh1106_SetCommandMode();
				ssh1106_SetPageStart((++pageIndex) % 8); // increment page, with wraparound
				if (align) {
					ssh1106_SetColumnStart(x + 0x2); // align wrapped text with initial start point
					charStart = x + 0x2;
				} else {
					ssh1106_SetColumnStart(0x2); // align wrapped text with screen
					charStart = 0x2;
				}
				ssh1106_SetDataMode();
			}
			charStart += 6;

			SSH1106_CS_SELECTED();
			for (int i = 0; i < 5; i++)
				ssh1106_spiIO(~font[i + word[j] * 5]);
			ssh1106_spiIO(0xFF);
			SSH1106_CS_DESELECTED();
		}
	}
}
