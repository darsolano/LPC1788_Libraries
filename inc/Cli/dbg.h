#ifndef __DBG_H__
#define __DBG_H__
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
    Revision Info:  $Id: dbg.h 13 2015-02-22 06:33:44Z pieterconradie $

============================================================================= */
/** 
    @ingroup UTILS
    @defgroup DBG dbg.h : Debug module

    Outputs debug information via printf

    File(s):
    - utils/dbg.h
    - utils/dbg_cfg_example.h
    - utils/dbg.c

    An old school debugging technique is to add debug print statements in the
    source code to track program flow and check for coding mistakes during
    development. These debug statements will be compiled in when the DBG symbol
    is defined in the Makefile (e.g. -DDBG=1) or on a per file basis by defining
    DBG before including 'utils/dbg.h'. The options are set in a project
    specific 'dbg_cfg.h'
 
    @warn_s
    After including "dbg.h" the macro DBG_DECL_NAME() *MUST* be invoked
    too or the compiler will output the following when debug output is
    enabled:
    @warn_e
 
        error: '_dbg_name' undeclared (first use in this function)    
 
    Correct way to declare module name:
 
        @code
        #include "utils/dbg.h"
        DBG_DECL_NAME(__FILE__); // Or DBG_DECL_NAME("module name or file name");
        @endcode
 
    @tip_s
    The code overhead can be dramatically reduced by defining DBG_CFG_NAME_LINE_ONLY = 1
    @tip_e
    
    Example:
 
    @include utils/test/dbg_test.c
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "defines.h"

// See if DBG symbol has been defined in Makefile or before #include "utils/dbg.h"
#if defined(DBG)

// Include project specific config. See "dbg_cfg_example.h"
#include "dbg_cfg.h"

#else

// DBG symbol not defined. Remove debugging code.
#define DBG                     0
#define DBG_CFG_LEVEL           DBG_CFG_LEVEL_NONE
#define DBG_CFG_NAME_LINE_ONLY  0
#define DBG_CFG_BUFFER_SIZE     32

#endif

// Check that all project specific options have been specified in "dbg_cfg.h"
#ifndef DBG
#error "DBG not specified"
#endif
#ifndef DBG_CFG_LEVEL
#error "DBG_CFG_LEVEL not specified"
#endif
#ifndef DBG_CFG_NAME_LINE_ONLY
#error "DBG_CFG_NAME_LINE_ONLY not specified"
#endif
#ifndef DBG_CFG_BUFFER_SIZE
#error "DBG_CFG_BUFFER_SIZE not specified"
#endif

/* _____DEFINITIONS _________________________________________________________ */
/// @name Debug level bitmask definitions
//@{
/// None
#define DBG_CFG_LEVEL_NONE   0
/// Errors
#define DBG_CFG_LEVEL_ERR    (1<<0)
/// Warnings
#define DBG_CFG_LEVEL_WARN   (1<<1)
/// Info
#define DBG_CFG_LEVEL_INFO   (1<<2)
/// All
#define DBG_CFG_LEVEL_ALL    (DBG_CFG_LEVEL_ERR|DBG_CFG_LEVEL_WARN|DBG_CFG_LEVEL_INFO)
//@}

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
   Debug print debug module name and line.

   @param file      File name
   @param line      Line number
 */
extern void dbg_print(const char * file, 
                      u16_t        line);

/**
   Debug print debug module name, line and variable argument user format string.
    
   @param file      File name
   @param line      Line number 
   @param format    User format string 
   @param ...       Variable number of arguments 
 */
extern void dbg_printf(const char * file, 
                       u16_t        line, 
                       const char * format, ...) ;

/* _____MACROS_______________________________________________________________ */
#if DBG

/// Declare debug module name once for each file to reduce code size
#define DBG_DECL_NAME(name) \
    static const char _dbg_name[] ATTR_PGM = name;


// Output debug module name and line number only?
#if (DBG_CFG_NAME_LINE_ONLY != 0)

// Output file and line number only (remove format string and variable arguments)
#define _DBG_PRINTF(file, line, format, ...) dbg_print(file, line)

#else

// Output full debug output, including format string and variable arguments
#define _DBG_PRINTF(file, line, format, ...) dbg_printf(file, line, format, ## __VA_ARGS__)

#endif

/**
    Macro that will output debug output if #DBG_CFG_LEVEL is defined as non zero.

    @note The format of this macro is specific to the GCC preprocessor.

    @param[in] format Format string following by a variable list of arguments.
    
 */
#define DBG_TRACE(format, ...) \
            do \
            { \
                if(DBG_CFG_LEVEL != 0) \
                { \
                    printf_P(PSTR(format), ## __VA_ARGS__); \
                } \
            } while (0)

/**
    Macro that will output debug output if the specified level bit is set in
    #DBG_CFG_LEVEL.

    The file name and line number is prepended to the format string to form one
    string.
    @note The format of this macro is specific to the GCC preprocessor.

    @param[in] level  The debug severity level (progress, warning or error)
    @param[in] format Format string following by a variable list of arguments.
    
 */
#define DBG_LOG(level, format, ...) \
            do \
            { \
                if(level & DBG_CFG_LEVEL) \
                { \
                    _DBG_PRINTF(_dbg_name, __LINE__, PSTR(format), ## __VA_ARGS__); \
                } \
            } while(0)

/**
    Macro that will test an expression, and block indefinitely if FALSE.

    This macro will perform the test and if FALSE, will output the filename and
    line number with the test appended. The macro will then block indefinitely.

    @param[in] expression   Expression that evaluates to a boolean value
                            (TRUE or FALSE)
 */
#define DBG_ASSERT(expression) \
            do \
            { \
                if((DBG_CFG_LEVEL != 0) && (!(expression))) \
                { \
                    _DBG_PRINTF(_dbg_name, __LINE__, PSTR("A " #expression)); \
                    for(;;) {;} \
                } \
            } while(0)

#else

// Remove debugging code
#define DBG_DECL_NAME(name)
#define DBG_TRACE(format, ...)
#define DBG_LOG(level, format, ...)
#define DBG_ASSERT(expression)

#endif

/**
   Shortcut macro to display an error message. 
    
   @see DBG_LOG
 */
#define DBG_ERR(format, ...)  DBG_LOG(DBG_CFG_LEVEL_ERR,  "E " format, ## __VA_ARGS__)

/**
   Shortcut macro to display a warning message. 
    
   @see DBG_LOG
 */
#define DBG_WARN(format, ...) DBG_LOG(DBG_CFG_LEVEL_WARN, "W " format, ## __VA_ARGS__)

/**
   Shortcut macro to display an info message. 
    
   @see DBG_LOG
 */
#define DBG_INFO(format, ...) DBG_LOG(DBG_CFG_LEVEL_INFO, "I " format, ## __VA_ARGS__)

/// @}
#endif
