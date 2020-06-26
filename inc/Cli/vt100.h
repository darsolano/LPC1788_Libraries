#ifndef __ANSI_ESC_H__
#define __ANSI_ESC_H__
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
    
    Title:          vt100.h : VT100/ANSI Terminal module
    Author(s):      Pieter Conradie
    Creation Date:  2012-09-15

============================================================================= */

/** 
    @ingroup COMMS
    @defgroup VT100 vt100.h : VT100/ANSI Terminal module
 
    Parses VT100 / ANSI escape sequences to interface with a terminal emulator.
    
    File(s):
    - comms/vt100.h
    - comms/vt100.c
    
    @see
    - http://en.wikipedia.org/wiki/ANSI_escape_code
    - http://www.termsys.demon.co.uk/vtansi.htm
    
    Example:
 
    @include comms/test/cli_test.c
  */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "defines.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS _________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// @name Special ASCII values
//@{
#define VT100_CHAR_BEL      0x07
#define VT100_CHAR_BS       0x08
#define VT100_CHAR_TAB      0x09
#define VT100_CHAR_CR       0x0D
#define VT100_CHAR_LF       0x0A
#define VT100_CHAR_ESC      0x1B
#define VT100_CHAR_DEL      0x7F
//@}

/// VT100 Terminal receive state
typedef enum
{
    VT100_CHAR_NORMAL,          ///< A normal key has been pressed and must be used
    VT100_CHAR_INVALID,         ///< An invalid key code has been sent and must be discarded
    VT100_ESC_SEQ_BUSY,         ///< Busy with escape sequence; data must be discarded
    VT100_ESC_SEQ_ARROW_UP,     ///< Up Arrow has been pressed
    VT100_ESC_SEQ_ARROW_DN,     ///< Down Arrow has been pressed
    VT100_ESC_SEQ_ARROW_LEFT,   ///< Left Arrow has been pressed
    VT100_ESC_SEQ_ARROW_RIGHT,  ///< Right Arrow has been pressed
} vt100_state_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/// Initialise module
extern void vt100_init(void);

/**
    Process a received character byte. 
    
    If an ANSI escape sequence is detected, VT100_ESC_SEQ_BUSY will be returned
    to indicate that received character should be ignored, otherwise
    VT100_CHAR_NORMAL is returned to indicate that the character must be used. 
    
    If an ANSI escape sequence is decoded it is indicated with a
    VT100_ESC_SEQ... state.
    
    @param data             Received character to be process for ANSI Escape Sequences
    
    @return vt100_state_t   VT100_CHAR_NORMAL if the character must be used as
                            normal; VT100_CHAR_INVALID or VT100_ESC_SEQ_BUSY if
                            character should be ignored; otherwise the decoded
                            ANSI Escape Sequence.
    
 */
extern vt100_state_t vt100_on_rx_char(char data);

/// Send 'clear screen' command to terminal
extern void vt100_clr_screen(void);

/// Send 'erase line' command to terminal
extern void vt100_erase_line(void);

/** 
    Delete specified number of characters
    
    @param nr_of_chars  Number of characters to delete
 */
extern void vt100_del_chars(u8_t nr_of_chars);

/* _____MACROS_______________________________________________________________ */

/// @}
#ifdef __cplusplus
}
#endif

#endif // #ifndef __ANSI_ESC_H__
