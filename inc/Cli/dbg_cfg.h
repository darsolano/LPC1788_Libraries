#ifndef __DBG_CFG_H__
#define __DBG_CFG_H__
/* =============================================================================

    Copyright (c) 2014 Pieter Conradie [http://piconomic.co.za]
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
    
    Title:          dbg_cfg.h : Debug module configuration
    Author(s):      Pieter Conradie
    Creation Date:  2014-01-17
    Revision Info:  $Id: dbg_cfg_example.h 13 2015-02-22 06:33:44Z pieterconradie $

============================================================================= */

/** 
    @addtogroup DBG
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "defines.h"

/* _____DEFINITIONS _________________________________________________________ */
#ifndef DBG
/// Flag to disable (DBG=0) or enable (DBG=1) debug.
#define DBG 0
#endif

#ifndef DBG_CFG_LEVEL
/**
   Global debug output level.

   It is a bitmask that sets which debug info will be emmitted. E.g.
   - DBG_CFG_LEVEL = DBG_CFG_LEVEL_NONE : No debug output
   - DBG_CFG_LEVEL = DBG_CFG_LEVEL_ERR  : Report errors only
   - DBG_CFG_LEVEL = (DBG_CFG_LEVEL_ERR|DBG_CFG_LEVEL_WARN) : Report errors + warnings
   - DBG_CFG_LEVEL = (DBG_CFG_LEVEL_ERR|DBG_CFG_LEVEL_WARN|DBG_CFG_LEVEL_INFO) : Report errors + warnings + info
 */
#define DBG_CFG_LEVEL DBG_CFG_LEVEL_ERR
#endif

#ifndef DBG_CFG_NAME_LINE_ONLY
/// Option to decrease debug footprint by displaying name and line only
#define DBG_CFG_NAME_LINE_ONLY 0
#endif

/// Debug output string buffer size
#define DBG_CFG_BUFFER_SIZE 32

/// @}
#endif
