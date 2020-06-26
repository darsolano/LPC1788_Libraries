#ifndef __AT45D_CFG_H__
#define __AT45D_CFG_H__
/* =============================================================================

    Copyright (c) 2014 Pieter Conradie <http://piconomix.com>
 
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
    
    Title:          at45d_cfg.h : AT45D Peripheral Driver configuration
    Author(s):      Pieter Conradie
    Creation Date:  2014-01-17

============================================================================= */

/** 
    @addtogroup AT45D
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "defines.h"

/* _____DEFINITIONS _________________________________________________________ */
/// Specify device
#define AT45D_CFG_DEVICE    AT45DB041

/**
    Select power of two page size.
    
    "Power of 2" binary page size Configuration Register is a user-programmable
    nonvolatile register that allows the page size of the main memory to be
    configured for binary page size or the DataFlash standard page size.
    
    See at45d_set_page_size_to_pwr_of_two().
 */
#define AT45D_CFG_PWR_OF_TWO_PAGE_SIZE    0

/// @}
#endif // #ifndef __AT45D_CFG_H__
