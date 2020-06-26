/* =============================================================================

    Copyright (c) 2012 Pieter Conradie <http://piconomix.com>
 
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    IN THE SOFTWARE.
    
    Title:          vt100.h : VT100/VT100 Terminal module
    Author(s):      Pieter Conradie
    Creation Date:  2012-09-15

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <stdio.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "Cli/vt100.h"
#include <timer.h>
#include <monitor.h>

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static u8_t vt100_state;

/* _____LOCAL FUNCTION PROTOTYPES____________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void vt100_init()
{
    // Reset state
    vt100_state = 0;

    // Reset terminal
    xputc(VT100_CHAR_ESC);
    xputc('c');

    // Enable line wrap
    xputc(VT100_CHAR_ESC);
    xputc('[');
    xputc('7');
    xputc('h');

    // Wait until Terminal has reset
    //_delay(dly_100ms);
}

vt100_state_t vt100_on_rx_char(char data)
{
    switch(vt100_state)
    {
    case 0:
        if(data == VT100_CHAR_ESC)
        {
            // Escape sequence detected
            vt100_state++;
            // Indicate that received character should be discarded
            return VT100_ESC_SEQ_BUSY;
        }
        if(data >= 0x80)
        {
            // Invalid character received
            return VT100_CHAR_INVALID;            
        }
        // Normal character received
        return VT100_CHAR_NORMAL;        

    case 1:
        if(data == '[')
        {
            // Escape sequence detected
            vt100_state++;
            // Indicate that received character should be ignored
            return VT100_ESC_SEQ_BUSY;
        }
        // Incorrect escape sequence
        vt100_state = 0;
        // Indicate that received character should be ignored
        return VT100_ESC_SEQ_BUSY;

    case 2:
        // Reset state first
        vt100_state = 0;

        // Detect sequence
        switch(data)
        {
        case 'A': return VT100_ESC_SEQ_ARROW_UP;
        case 'B': return VT100_ESC_SEQ_ARROW_DN;
        case 'C': return VT100_ESC_SEQ_ARROW_LEFT;
        case 'D': return VT100_ESC_SEQ_ARROW_RIGHT;
        default:  return VT100_CHAR_INVALID;
        }

    default:
        //Reset state
        vt100_state = 0;
        // Indicate that received character should be discarded
        return VT100_CHAR_INVALID;
    }
}

void vt100_clr_screen(void)
{
	xputc(VT100_CHAR_ESC);
	xputc('[');
	xputc('2');
	xputc('J');
}

void vt100_erase_line(void)
{
	xputc(VT100_CHAR_ESC);
	xputc('[');
	xputc('2');
	xputc('K');
}

void vt100_del_chars(u8_t nr_of_chars)
{
    while(nr_of_chars != 0)
    {
    	xputc(VT100_CHAR_BS);
    	xputc(' ');
    	xputc(VT100_CHAR_BS);
        nr_of_chars--;
    }
}
