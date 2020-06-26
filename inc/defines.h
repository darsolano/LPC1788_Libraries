#ifndef __DEFINES_H__
#define __DEFINES_H__
/* =============================================================================

    Copyright (c) 2006 - 2012 Pieter Conradie [http://piconomic.co.za]
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
    
    Title:          defines.h : Common definitions
    Author(s):      Pieter Conradie
    Creation Date:  2008-02-05
    Revision Info:  $Id: defines.h 13 2015-02-22 06:33:44Z pieterconradie $

============================================================================= */
/** 
    @ingroup COMMON
    @defgroup DEFINES defines.h : Common definitions

    Definition of standard types, boolean, scope and utility macros.

    File:
    - common/defines.h
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

#ifndef __STD_TYPES_ABSENT__
#include <stdint.h>
#include <stdbool.h>
#endif

/* _____PROJECT INCLUDES_____________________________________________________ */

/* _____DEFINITIONS _________________________________________________________ */
#ifndef NULL
/// NULL pointer
#define NULL 0
#endif

/* _____TYPE DEFINITIONS_____________________________________________________ */
#ifdef __STD_TYPES_ABSENT__
// Provide standard types if absent from standard C library
typedef unsigned char   uint8_t;
typedef signed char     int8_t;
typedef unsigned short  uint16_t;
typedef signed short    int16_t;
typedef unsigned long   uint32_t;
typedef signed long     int32_t;

#define _Bool uint8_t

#endif

/// @name Standard types
//@{
typedef uint8_t  u8_t;     ///< unsigned 8-bit value (0 to 255)
typedef int8_t   s8_t;     ///< signed 8-bit value (-128 to +127)
typedef uint16_t u16_t;    ///< unsigned 16-bit value (0 to 65535)
typedef int16_t  s16_t;    ///< signed 16-bit value (-32768 to 32767)
typedef uint32_t u32_t;    ///< unsigned 32-bit value (0 to 4294967296)
typedef int32_t  s32_t;    ///< signed 32-bit value (-2147483648 to +2147483647)
typedef float    float_t;  ///< 32 bits precision floating point number
typedef double   double_t; ///< 64 bits precision floating point number
//@}

/// @name Boolean type
//@{
#ifdef __cplusplus

#define bool_t bool
#ifndef FALSE
#define FALSE  false
#endif
#ifndef TRUE
#define TRUE   true
#endif

#else

#define bool_t _Bool
#ifndef FALSE
//#define FALSE  0
#endif
#ifndef TRUE
//#define TRUE   (!FALSE)
#endif

#endif
//@}

/* _____MACROS_______________________________________________________________ */
/// @name Minimum and maximum of standard types
//@{
#define U8_MIN      0x00            ///< Minimum of unsigned 8-bit value
#define U8_MAX      0xff            ///< Maximum of unsigned 8-bit value
#define U16_MIN     0x0000          ///< Minimum of unsigned 16-bit value
#define U16_MAX     0xffff          ///< Maximum of unsigned 16-bit value
#define U32_MIN     0x00000000      ///< Minimum of unsigned 32-bit value
#define U32_MAX     0xffffffff      ///< Maximum of unsigned 32-bit value
#define S8_MIN      (-0x80)         ///< Minimum of signed 8-bit value
#define S8_MAX      0x7f            ///< Maximum of signed 32-bit value
#define S16_MIN     (-0x8000)       ///< Minimum of signed 16-bit value
#define S16_MAX     0x7fff          ///< Maximum of signed 32-bit value
#define S32_MIN     (-0x80000000)   ///< Minimum of signed 32-bit value
#define S32_MAX     0x7fffffff      ///< Maximum of signed 32-bit value
//@}

/**
   @name Type size and sign macros

   Source: [Catching Integer Overflows in C] (http://www.fefe.de/intof.html)
 */
//@{
///@cond INTERNAL
#define __HALF_MAX_SIGNED(type) ((type)1 << (sizeof(type)*8-2))
#define __MAX_SIGNED(type) (__HALF_MAX_SIGNED(type) - 1 + __HALF_MAX_SIGNED(type))
#define __MIN_SIGNED(type) (-1 - __MAX_SIGNED(type))
///@endcond

/// Test if a type is signed or unsigned
#define TYPE_IS_SIGNED(type)((type)-1 < 1)

/**
    Return the minimum of a type.

    For an unsigned type it is zero.
    For a signed type this is the largest negative value
 */
#define MIN_OF_TYPE(type) (TYPE_IS_SIGNED(type)?__MIN_SIGNED(type):(type)0)

///  Return the maximum of a type.
#define MAX_OF_TYPE(type) ((type)~MIN_OF_TYPE(type))
//@}

/// @name Concatenation macros
//@{
#define _CONCAT(x, y) x ## y

/** 
    Recursive token concatenation macro.
    
    Example:
        @code
        #define XY       123456
        #define TOKEN1   X
        #define TOKEN2   Y
        #define TOKEN1_2 CONCAT(TOKEN1,TOKEN2)
        @endcode

    - TOKEN1_2 is replaced by the C preprocessor with
    - CONCAT(TOKEN1,TOKEN2), which is replaced with
    - _CONCAT(X,Y), which is replaced with
    - XY, which is replaced with
    - 123456    
 */
#define CONCAT(x, y) _CONCAT(x, y)
//@}

/// @name Bit manipulation macros useful for example to manipulate GPIO pins
//@{
/// Set a bit (1)
#define BIT_SET_HI(var,bit)             do {(var) |= (1<<(bit));} while(0)

/// Clear a bit (0)
#define BIT_SET_LO(var,bit)             do {(var) &= ~(1<<(bit));} while(0)

/// Toggle a bit
#define BIT_TOGGLE(var,bit)             do {(var) ^= (1<<(bit));} while(0)

/// Test if a bit is set (1?)
#define BIT_IS_HI(var,bit)              (((var)&(1<<(bit))) != 0)

/// Test if a bit is cleared (0?)
#define BIT_IS_LO(var,bit)              (((var)&(1<<(bit))) == 0)

/// Wait until a bit is set
#define LOOP_UNTIL_BIT_IS_HI(var,bit)   do {;} while(BIT_IS_LO((var),(bit)))

/// Wait until a bit is cleared
#define LOOP_UNTIL_BIT_IS_LO(var,bit)   do {;} while(BIT_IS_HI((var),(bit)))
//@}

/// @name Byte extraction macros
//@{
/// Extract the high 8 bits of a 16-bit value (Most Significant Byte)
#define U16_HI8(data) ((u8_t)(((data)>>8)&0xff))

/// Extract the low 8 bits of a 16-bit value (Least Significant Byte)
#define U16_LO8(data) ((u8_t)((data)&0xff))

/// Extract the high 8 bits (bits 31..24) of a 32-bit value
#define U32_HI8(data) ((u8_t)(((data)>>24)&0xff))

/// Extract the medium high 8 bits (bits 23..16) of a 32-bit value
#define U32_MH8(data) ((u8_t)(((data)>>16)&0xff))

/// Extract the medium low 8 bits (bits 15..8) of a 32-bit value
#define U32_ML8(data) ((u8_t)(((data)>>8)&0xff))

/// Extract the low 8 bits (bits 7..0) of a 32-bit value
#define U32_LO8(data) ((u8_t)((data)&0xff))
//@}

/// @name Bit concatenation macros
//@{
/// Concatenate 8 bits to form 8-bit value
#define U8_CONCAT_U1(b7, b6, b5, b4, b3, b2, b1, b0) \
                        ( ((b7)<<7)+((b6)<<6)+((b5)<<5)+((b4)<<4)+ \
                          ((b3)<<3)+((b2)<<2)+((b1)<<1)+((b0)<<0) )

//@}

/// @name Byte concatenation macros
//@{
/// Concatenate 2 x 8 bits to form 16-bit value
#define U16_CONCAT_U8(hi8, lo8) ( (((u16_t)(hi8))<<8) \
                                 +((u16_t)(lo8))     )

/// Concatenate 4 x 8 bits to form 32-bit value
#define U32_CONCAT_U8(hi8, mh8, ml8, lo8) ( (((u32_t)(hi8))<<24) \
                                           +(((u32_t)(mh8))<<16) \
                                           +(((u32_t)(ml8))<<8 ) \
                                           +((u32_t)(lo8))      )
//@}

/// @name General utility macros
//@{

/// Calculate unsigned division with rounding to nearest integer value
#define UDIV_ROUND(dividend,divisor)        (((dividend)+((divisor)>>1))/(divisor))

/// Calculate unsigned division with rounding to nearest integer value
#define UDIV_ROUNDUP(dividend,divisor)      (((dividend)+((divisor)-1))/(divisor))

/// Calculate the number of items in an array
#define SIZEOF_ARRAY(array)                   (sizeof(array)/sizeof((array)[0]))

/// See if a value is a power of two
#define VAL_IS_PWR_OF_TWO(value)            (((value)&((value)-1)) == 0)

//@}

/// @}
#endif
