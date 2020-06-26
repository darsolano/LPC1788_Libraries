/*
 * ssh1106.h
 *
 *  Created on: Jan 29, 2019
 *      Author: dsolano
 */

#ifndef INC_OLED_SSH1106_H_
#define INC_OLED_SSH1106_H_

#include <chip.h>
#include <lpc_types.h>
#include <stdint.h>

#define SSH1106SSPx					LPC_SSP0


#define SSH1106_DISPLAY_WIDTH          128
#define SSH1106_DISPLAY_HEIGHT         32
#define X_OFFSET 2

typedef enum
{
    SSH1106_COLOR_BLACK,
    SSH1106_COLOR_WHITE
} ssh1106_color_t;


void ssh1106_Init(void);
void ssh1106_clear_screen(ssh1106_color_t color);
void ssh1106_DrawLine(int x, int y, int length, bool horizontal, ssh1106_color_t color);
void ssh1106_WriteChar(int x, int y, char c);
void ssh1106_WriteWord(int x, int y, int length, char* word, bool align);



#endif /* INC_OLED_SSH1106_H_ */
