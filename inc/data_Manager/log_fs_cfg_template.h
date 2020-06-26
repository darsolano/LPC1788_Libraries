#ifndef __LOG_FS_CFG_H__
#define __LOG_FS_CFG_H__
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
    
    Title:          log_fs_cfg.h : Log file system configuration
    Author(s):      Pieter Conradie
    Creation Date:  2014-06-09

============================================================================= */

/** 
    @addtogroup LOG_FS
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "defines.h"
#include "at45d.h"

/* _____DEFINITIONS _________________________________________________________ */
/// Page size for file system
#define LOG_FS_CFG_PAGE_SIZE        AT45D_PAGE_SIZE

/// Start page for file system
#define LOG_FS_CFG_PAGE_START       0

/// End page for file system
#define LOG_FS_CFG_PAGE_END         (AT45D_PAGES-1)

/// Record data size
#define LOG_FS_CFG_REC_DATA_SIZE    13

/// Log type
#define LOG_FS_CFG_TYPE             LOG_FS_CFG_TYPE_LINEAR

/// Maximum number of pages allocated to file. 0 means no limit
#define LOG_FS_CFG_MAX_PAGES        0

/// @}
#endif // #ifndef __LOG_FS_CFG_H__
