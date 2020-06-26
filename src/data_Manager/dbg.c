/* =============================================================================

    Copyright (c) 2006 Pieter Conradie [http://piconomic.co.za]
    All rights reserved.
    
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met: 
    
    1. Redistributions of source code must retain the above copyright notice, 
       this list of conditions and the following disclaimer. 
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution. 
    3. Credit must appear prominently in all internet publications (including
       advertisements) of products or projects that use this software by
       including the following text and URL:
       "Uses Piconomic FW Library <http://piconomic.co.za>"
 
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
    POSSIBILITY OF SUCH DAMAGE.

    Title:          dbg.h : Debug module
    Author(s):      Pieter Conradie
    Creation Date:  2007-03-31
    Revision Info:  $Id: dbg.c 13 2015-02-22 06:33:44Z pieterconradie $

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <stdio.h>
#include <stdarg.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "data_Manager/dbg.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
#ifdef COMPILER_GCC_AVR

void dbg_print(const char * file, u16_t line) 
{
    // Output file and line
    printf_P(file);
    PRINTF_P(" %d\n", line);
}

void dbg_printf(const char * file, u16_t line, const char * format, ...) 
{
    va_list args;
    char dbg_buffer[DBG_CFG_BUFFER_SIZE];

    // Output file and line
    printf_P(file);
    PRINTF_P(" %d : ", line);

    // Start variable argument list
    va_start(args, format);

    // Populate buffer with debug string
    vsnprintf_P(dbg_buffer, DBG_CFG_BUFFER_SIZE, format, args);
 
    // End variable argument list
    va_end(args);

    // Append new line and terminating zero in case of buffer overrun
    dbg_buffer[DBG_CFG_BUFFER_SIZE-2] = '\n';
    dbg_buffer[DBG_CFG_BUFFER_SIZE-1] = '\0';

    // Output user formatted string
   printf(dbg_buffer);
}

#else

void dbg_print(const char * file, u16_t line)
{
    // Output file and line
    printf("%s %d\n",file, line);
}

void dbg_printf(const char * file, u16_t line, const char * format, ...) 
{
    va_list args;
    char dbg_buffer[DBG_CFG_BUFFER_SIZE];

    // Output file and line
    printf("%s %d : ",file, line);

    // Start variable argument list
    va_start(args, format);

    // Populate buffer with debug string
    vsnprintf(dbg_buffer, DBG_CFG_BUFFER_SIZE, format, args);
 
    // End variable argument list
    va_end(args);

    // Append terminating zero in case of buffer overrun
    dbg_buffer[DBG_CFG_BUFFER_SIZE-1] = '\0';

    // Output user formatted string
    printf(dbg_buffer);
}

#endif
