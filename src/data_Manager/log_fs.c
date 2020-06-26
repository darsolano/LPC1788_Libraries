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
 
    Title:          log_fs.h : Simple record-based file system for Serial Flash
    Author(s):      Pieter Conradie
    Creation Date:  2014-06-09

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <string.h>
#include <stddef.h>
#include <stdio.h>
/* _____PROJECT INCLUDES_____________________________________________________ */
#include "data_Manager/log_fs.h"
#include "data_Manager/at45d.h"

#include "data_Manager/dbg.h"
DBG_DECL_NAME("log_fs")

/* _____LOCAL DEFINITIONS____________________________________________________ */
/// Invalid page value
#define LOG_FS_PAGE_INVALID         0xffff

#if (LOG_FS_CFG_PAGE_END >= LOG_FS_PAGE_INVALID)
#error "LOG_FS_PAGE_INVALID must not be a valid page value"
#endif

/// File attribute data size (page size - file block size)
#define LOG_FS_FILE_ATTR_DATA_SIZE (LOG_FS_CFG_PAGE_SIZE - sizeof(log_fs_file_info_t))

/// Record page data size (page size - page header size)
#define LOG_FS_REC_PAGE_DATA_SIZE (LOG_FS_CFG_PAGE_SIZE - sizeof(log_fs_page_header_t))

/// Records per page
#define LOG_FS_RECORDS_PER_PAGE (LOG_FS_REC_PAGE_DATA_SIZE / sizeof(log_fs_rec_block_t))

/// First record offset
#define LOG_FS_REC_OFFSET_FIRST (sizeof(log_fs_page_header_t))

/// Last record offset
#define LOG_FS_REC_OFFSET_LAST  (sizeof(log_fs_page_header_t) + (LOG_FS_RECORDS_PER_PAGE-1) * sizeof(log_fs_rec_block_t))

/// Marker size definition
typedef u8_t log_fs_marker_t;

/// @name Marker values
//@{
#define LOG_FS_MARKER_FREE      0xff /* 1111 1111b */
#define LOG_FS_MARKER_FILE      0x2f /* 0010 1111b */
#define LOG_FS_MARKER_RECORD    0x1f /* 0001 1111b */
#define LOG_FS_MARKER_BAD       0x00 /* 0000 0000b */
//@}

/// Specification of data address in Serial Flash
typedef struct
{
    log_fs_page_t   page;       ///< Serial Flash page
    log_fs_offset_t offset;     ///< Offset inside page
} log_fs_adr_t;

/// /// Rolling number size definition
typedef u16_t log_fs_nr_t;

/// CRC size definition
typedef u8_t log_fs_crc_t;

/// Specification of page header that is stored at the start of each page
typedef struct
{
    log_fs_marker_t marker;     ///< FREE, FILE, RECORD or BAD
    log_fs_nr_t     nr;         ///< Rolling number to find FILE or RECORD start and end
    log_fs_crc_t    crc;        ///< CRC checksum
} __attribute__((__packed__)) log_fs_page_header_t;

/// Specification of file info (including CRC) that is stored after a FILE page header
typedef struct
{
    log_fs_file_t        file;          ///< File info (page number and time stamp)
    log_fs_crc_t         crc;           ///< CRC checksum
} __attribute__((__packed__)) log_fs_file_info_t;

/// Specification of a record block written to Serial Flash
typedef struct
{
    log_fs_marker_t marker;                             ///< Record marker
    u8_t            data[LOG_FS_CFG_REC_DATA_SIZE];     ///< Record data content
    log_fs_crc_t    crc;                                ///< CRC checksum
} __attribute__((__packed__)) log_fs_rec_block_t;

/// File data and state
typedef struct
{
    log_fs_page_header_t page_header;       ///< Page header
    log_fs_file_info_t   file_info;         ///< File info
    log_fs_rec_block_t   record_block;      ///< Record block

    log_fs_page_t        file_page_first;   ///< Page number of first file (LOG_FS_PAGE_INVALID if empty)
    log_fs_page_t        file_page_last;    ///< Page number of last file (LOG_FS_PAGE_INVALID if empty)
    log_fs_page_t        file_page_nr_next; ///< Next file page number to use (starts at 0)

    log_fs_page_t        rec_page_first;    ///< First page with records (LOG_FS_PAGE_INVALID if empty)
    log_fs_page_t        rec_page_last;     ///< Last page with records (LOG_FS_PAGE_INVALID if empty)
    log_fs_page_t        rec_page_nr_next;  ///< Next record page number to use (starts at 0)

    log_fs_adr_t         rec_adr_rd;        ///< Current read address  (page = LOG_FS_PAGE_INVALID when file is opened)
    log_fs_adr_t         rec_adr_wr;        ///< Next write address (open position)
} log_fs_t;

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static log_fs_t log_fs;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */
/**
    Get next page number.

    The Serial Flash pages are treated like a circular buffer. If the end page 
    number is specified, the first page number is returned.

    @param  page            The current page number
    @return log_fs_page_t   The next page number
 */
static log_fs_page_t log_fs_page_next(log_fs_page_t page) ;

/**
    Get previous page number.

    The Serial Flash pages are treated like a circular buffer. If the first page
    number is specified, the last page number is returned.

    @param  page            The current page number
    @return log_fs_page_t   The previous page number
 */
static log_fs_page_t log_fs_page_previous(log_fs_page_t page) ;

/**
    Calculate a CRC (checksum) over a specified block of bytes.
    
    @param data             Pointer to a buffer containing the data
    @param nr_of_bytes      Number of bytes to calculate 
    
    @return log_fs_crc_t    Calculated CRC
 */
static log_fs_crc_t log_fs_crc_calc(const void * data, size_t nr_of_bytes);

/**
    Calculate CRC (checksum) of the page header that is stored at the start of 
    the page.
    
    @return log_fs_crc_t    Calculated CRC
 */
static log_fs_crc_t log_fs_crc_page_header(void);

/**
    Calculate CRC (checksum) of the file info (page number and time stamp).
    
    @return log_fs_crc_t    Calculated CRC
 */
static log_fs_crc_t log_fs_crc_file_info(void);

/**
    Calculate CRC (checksum) of the record block.
    
    @return log_fs_crc_t    Calculated CRC
 */
static log_fs_crc_t log_fs_crc_record_block(void);

/**
    Read a marker at a specified page and offset.

    The marker is checked to see if it is valid. If it is invalid, it is
    overwritten as BAD (0x00).
    
    @param page                 Page to read
    @param offset               Offset to read
    
    @return log_fs_marker_t     Marker value
 */
static log_fs_marker_t log_fs_marker_rd(log_fs_page_t   page,
                                        log_fs_offset_t offset);

/**
    Write a marker to the specified page and offset.
    
    @param marker   Marker value to write
    @param page     Page to write
    @param offset   Offset to write
    
    @retval TRUE    Marker was succesfully written and verified
    @retval FALSE   Marker failed to write correctly and was overwritten as BAD
                    (0x00).
 */
static bool_t log_fs_marker_wr(log_fs_marker_t marker,
                               log_fs_page_t   page,
                               log_fs_offset_t offset);

/**
    Read a page header from the start of the specified page.
    
    A page header is read from the specified page. If the CRC check failed, the
    marker will be overwritten to BAD (0x00) to invalidate it.

    @param page             Specified page to read from
    
    @return log_fs_marker_t Marker of page header. BAD if CRC check failed
 */
static log_fs_marker_t log_fs_page_header_rd(log_fs_page_t page);

/**
    Write a page header to the start of the specified page.

    A page header is written to the specified page and verified. If the 
    verification failes, the marker is overwritten to BAD (0x00) to invalidate
    it.
    
    @param page             Page to write to
    
    @retval TRUE            Page header written correctly
    @retval FALSE           Page header write failed and marker set to BAD 
 */
static bool_t log_fs_page_header_wr(log_fs_page_t page);

/**
    Read a file block (page header & file info) from the specified page.
    
    @param page             Specified page to read from
    
    @retval TRUE            Page contains a valid file block
    @retval FALSE           Page does not contain a valid file block
 */
static bool_t log_fs_file_block_rd(log_fs_page_t page);

/**
    Write a file block (page header & file info) to the specified page.

    A file block is written to the specified page and verified. If the 
    verification failes, the marker is overwritten to BAD (0x00) to invalidate
    it.
    
    @param page             Page to write to
    
    @retval TRUE            Page header written correctly
    @retval FALSE           Page header write failed and marker set to BAD 
 */
static bool_t log_fs_file_block_wr(log_fs_page_t page);

/**
    Read a record block from the specified page and offset.

    If the CRC is invalid, the marker is overwritten to BAD (0x00) to invalidate
    it.
    
    @param page             Specified page to read from
    @param offset           Specified offset to read from
    
    @retval TRUE            Valid record block read
    @retval FALSE           Record block not valid
 */
static bool_t log_fs_record_block_rd(log_fs_page_t   page,
                                     log_fs_offset_t offset);
/**
    Write a record block to the specified page and offset.

    The block is verified and the marker is overwritten to BAD if verification 
    failed.
    
    @param page             Specified page to write to
    @param offset           Specfied offset to write to
    
    @retval TRUE            Record block succesfully written
    @retval FALSE           Failed to write record block and marker has been 
                            overwritten to BAD
 */
static bool_t log_fs_record_block_wr(log_fs_page_t   page,
                                     log_fs_offset_t offset);

/**
    Given the start page of a file, this function calculates the first record 
    page that may be written to (the start boundary). It is the next page.
 
    @return log_fs_page_t   The first record page that may be written to
 */
static log_fs_page_t log_fs_record_pages_bound_start(void);

/**
    Given the start page of a file, this function calculates the last record 
    page that may be written to (the end boundary).

    If the number of record pages is not limited (LOG_FS_CFG_MAX_PAGES == 0) 
    then the last record page that may be written to is just before the file 
    page due to the circular nature of the file system.
  
    @return log_fs_page_t   The last record page that may be written to
 */
static log_fs_page_t log_fs_record_pages_bound_end(void);

/**
    Find page number of the first page that contains the specified marker.
    
    @param marker           Specified marker to search for (e.g.
                            LOG_FS_MARKER_FILE or LOG_FS_MARKER_RECORD).
    @param page_start       Start page to search for specified marker
    @param page_end         End page to search for specified marker
    
    @return log_fs_page_t   Page number containing specified marker or
                            LOG_FS_PAGE_INVALID if page could not be found.
 */
static log_fs_page_t log_fs_page_header_find_first(log_fs_marker_t marker,
                                                   log_fs_page_t   page_start, 
                                                   log_fs_page_t   page_end);

/**
    Find page number of the next page that contains the specified marker.
    
    @param marker           Specified marker to search for (e.g.
                            LOG_FS_MARKER_FILE or LOG_FS_MARKER_RECORD).
    @param page_start       Start page to search for specified marker
    @param page_end         End page to search for specified marker
    @param page_current     Current page that contains a valid marker
    
    @return log_fs_page_t   Page number containing specified marker or
                            LOG_FS_PAGE_INVALID if page could not be found.
 */
static log_fs_page_t log_fs_page_header_find_next(log_fs_marker_t marker,
                                                  log_fs_page_t   page_start, 
                                                  log_fs_page_t   page_end,
                                                  log_fs_page_t   page_current);

/* _____LOCAL FUNCTIONS______________________________________________________ */
static log_fs_page_t log_fs_page_next(log_fs_page_t page)
{
    // Sanity check
    DBG_ASSERT( (page >= LOG_FS_CFG_PAGE_START) && (page <= LOG_FS_CFG_PAGE_END) );

    // End?
    if(page == LOG_FS_CFG_PAGE_END)
    {
        // Wrap to start
        return LOG_FS_CFG_PAGE_START;
    }
    else
    {
        // Return next page number
        return ++page;
    }
}

static log_fs_page_t log_fs_page_previous(log_fs_page_t page)
{
    // Sanity check
    DBG_ASSERT( (page >= LOG_FS_CFG_PAGE_START) && (page <= LOG_FS_CFG_PAGE_END) );

    // Start?
    if(page == LOG_FS_CFG_PAGE_START)
    {
        // Wrap to end
        return LOG_FS_CFG_PAGE_END;
    }
    else
    {
        // Return previous page number
        return --page;
    }
}

static log_fs_crc_t log_fs_crc_calc(const void * data, size_t nr_of_bytes)
{
    u8_t         i;
    u8_t *       data_u8 = (u8_t *)data;
    log_fs_crc_t crc = 0xff;

    // Repeat until all the data bytes have been processed...
    while(nr_of_bytes != 0)
    {
        nr_of_bytes--;

        // XOR CRC with 8-bit data
        crc = crc ^ (*data_u8++);

        // Repeat 8 times (for each bit)
        for(i=8; i!=0; i--)
        {
            // Is lowest bit set?
            if((crc & 1) != 0)
            {
                // Shift right and XOR with reverse of polynomial x^8+x^6+x^3+x^2+x^0
                crc = (crc >> 1) ^ 0xb2;
            }
            else
            {
                // Shift right
                crc = (crc >> 1);
            }
        }
    }
	return crc;
}

static log_fs_crc_t log_fs_crc_page_header(void)
{
    // Calculate CRC over whole block, except for CRC part
    return log_fs_crc_calc(&log_fs.page_header, offsetof(log_fs_page_header_t, crc));
}

static log_fs_crc_t log_fs_crc_file_info(void)
{
    // Calculate CRC over whole block, except for CRC part
    return log_fs_crc_calc(&log_fs.file_info, offsetof(log_fs_file_info_t, crc));
}

static log_fs_crc_t log_fs_crc_record_block(void)
{
    // Calculate CRC over whole block, except for CRC part
    return log_fs_crc_calc(&log_fs.record_block, offsetof(log_fs_rec_block_t, crc));
}

static log_fs_marker_t log_fs_marker_rd(log_fs_page_t   page,
                                        log_fs_offset_t offset)
{
    log_fs_marker_t marker;

    // Read marker
    at45d_rd_page_offset(&marker,
                         page,
                         offset,
                         sizeof(log_fs_marker_t));

    // Invalid marker?
    if(  (marker != LOG_FS_MARKER_FREE  )
       &&(marker != LOG_FS_MARKER_FILE  )
       &&(marker != LOG_FS_MARKER_RECORD)
       &&(marker != LOG_FS_MARKER_BAD   )  )
    {
        DBG_ERR("Invalid marker 0x%02X (page %u, offset %u)", marker, page, offset);

        // Set marker to BAD
        marker = LOG_FS_MARKER_BAD;
        at45d_wr_page_offset(&marker,
                             page,
                             offset,
                             sizeof(log_fs_marker_t));
    }

    return marker;
}

static bool_t log_fs_marker_wr(log_fs_marker_t marker,
                               log_fs_page_t   page,
                               log_fs_offset_t offset)
{
    log_fs_marker_t marker_rd;

    // Write marker
    at45d_wr_page_offset(&marker,
                         page,
                         offset,
                         sizeof(log_fs_marker_t));

    // Marker correctly written?
    marker_rd = log_fs_marker_rd(page, offset);
    if(marker_rd != marker)
    {
        DBG_ERR("Could not write marker(page %u, offset %u): wr 0x%02X, rd 0x%02X",
                page, offset, marker, marker_rd);
        // Set marker to BAD
        marker = LOG_FS_MARKER_BAD;
        at45d_wr_page_offset(&marker,
                             page,
                             offset,
                             sizeof(log_fs_marker_t));
        // Failure
        return FALSE;
    }
    else
    {
        // Success
        return TRUE;
    }
}

static log_fs_marker_t log_fs_page_header_rd(log_fs_page_t page)
{
    log_fs_marker_t marker;
    log_fs_crc_t    crc;

    // Read marker (and set to BAD if invalid value)
    marker = log_fs_marker_rd(page, 0);
    if(  (marker == LOG_FS_MARKER_FREE) || (marker == LOG_FS_MARKER_BAD)  )
    {
        return marker;
    }

    // Read page header
    at45d_rd_page_offset(&log_fs.page_header,
                         page,
                         0,
                         sizeof(log_fs_page_header_t));

    // Check CRC
    crc = log_fs_crc_page_header();
    if(crc != log_fs.page_header.crc)
    {
        // Mark page header as BAD
        DBG_ERR("Page header CRC check failed (page %u): wr 0x%02X, rd 0x%02X",
                page, log_fs.page_header.crc, crc);
        log_fs_marker_wr(LOG_FS_MARKER_BAD, page, 0);
        return LOG_FS_MARKER_BAD;
    }

    // Return marker
    return log_fs.page_header.marker;
}

static bool_t log_fs_page_header_wr(log_fs_page_t page)
{
    log_fs_page_header_t page_header_rd;

    // Calculate CRC
    log_fs.page_header.crc = log_fs_crc_page_header();

    DBG_INFO("Writing page header (page %u, marker 0x%02X, nr %u, crc 0x%02X)",
             page, log_fs.page_header.marker, log_fs.page_header.nr, log_fs.page_header.crc);

    // Write page header
    at45d_wr_page_offset(&log_fs.page_header,
                         page,
                         0,
                         sizeof(log_fs_page_header_t));

    // Read back page header
    at45d_rd_page_offset(&page_header_rd,
                         page,
                         0,
                         sizeof(log_fs_page_header_t));

    // Match?
    if(memcmp(&log_fs.page_header, &page_header_rd, sizeof(log_fs_page_header_t)) == 0)
    {
        // Success
        return TRUE;
    }

    // Mark page header as BAD
    DBG_ERR("Page header write failed (page %u)", page);
    log_fs_marker_wr(LOG_FS_MARKER_BAD, page, 0);

    // Failure
    return FALSE;
}

static bool_t log_fs_file_block_rd(log_fs_page_t page)
{
    // Read page header (and set to BAD if invalid)
    if(log_fs_page_header_rd(page) != LOG_FS_MARKER_FILE)
    {
        return FALSE;
    }

    // Read file info (located after page header)
    at45d_rd_page_offset(&log_fs.file_info,
                         page,
                         sizeof(log_fs_page_header_t),
                         sizeof(log_fs_file_info_t));

    // CRC correct?
    if(log_fs.file_info.crc != log_fs_crc_file_info())
    {
        // Mark as BAD
        DBG_ERR("File block CRC check failed (page %u)", page);
        log_fs_marker_wr(LOG_FS_MARKER_BAD, page, 0);
        return FALSE;
    }

    // Start page match?
    if(log_fs.file_info.file.start_page != page)
    {
        // Mark as BAD
        DBG_ERR("File block start page does not match (page %u)", page);
        log_fs_marker_wr(LOG_FS_MARKER_BAD, page, 0);
        return FALSE;
    }

    // File block valid
    return TRUE;
}

static bool_t log_fs_file_block_wr(log_fs_page_t page)
{
    log_fs_file_info_t file_block_rd;

    // Set page marker
    log_fs.page_header.marker = LOG_FS_MARKER_FILE;
    // Write page header
    if(!log_fs_page_header_wr(page))
    {
        return FALSE;
    }

    // Set start page
    log_fs.file_info.file.start_page = page;
    // Set file block CRC
    log_fs.file_info.crc = log_fs_crc_file_info();
    // Write file block
    at45d_wr_page_offset(&log_fs.file_info,
                         page,
                         sizeof(log_fs_page_header_t),
                         sizeof(log_fs_file_info_t));
    // Read back file block
    at45d_rd_page_offset(&file_block_rd,
                         page,
                         sizeof(log_fs_page_header_t),
                         sizeof(log_fs_file_info_t));
    // Match?
    if(memcmp(&log_fs.file_info, &file_block_rd, sizeof(log_fs_file_info_t)) == 0)
    {
        // Success
        return TRUE;
    }

    // Mark page as BAD
    DBG_ERR("File info write failed (page %u)", page);
    log_fs_marker_wr(LOG_FS_MARKER_BAD, page, 0);

    // Failure
    return FALSE;
}

static bool_t log_fs_record_block_rd(log_fs_page_t   page,
                                     log_fs_offset_t offset)
{
    // Read marker (and set to BAD if invalid value)
    if(log_fs_marker_rd(page, offset) != LOG_FS_MARKER_RECORD)
    {
        return FALSE;
    }
    // Read record entry
    at45d_rd_page_offset(&log_fs.record_block,
                         page,
                         offset,
                         sizeof(log_fs_rec_block_t));
    // CRC correct?
    if(log_fs.record_block.crc != log_fs_crc_record_block())
    {
        // Mark record as BAD
        DBG_ERR("Record block CRC check failed (page %u, offset %u)", page, offset);
        log_fs_marker_wr(LOG_FS_MARKER_BAD, page, offset);
        return FALSE;
    }

    // Record valid
    return TRUE;
}

static bool_t log_fs_record_block_wr(log_fs_page_t   page,
                                     log_fs_offset_t offset)
{
    log_fs_rec_block_t record_block_rd;

    // Sanity check
    if(offset + sizeof(log_fs_rec_block_t) >= LOG_FS_CFG_PAGE_SIZE)
    {
        DBG_ERR("Record block will overflow the page");
        return FALSE;
    }

    // Set record block marker
    log_fs.record_block.marker = LOG_FS_MARKER_RECORD;
    // Set record block CRC
    log_fs.record_block.crc = log_fs_crc_record_block();
    // Write record entry
    at45d_wr_page_offset(&log_fs.record_block,
                         page,
                         offset,
                         sizeof(log_fs_rec_block_t));
    // Read back record entry
    at45d_rd_page_offset(&record_block_rd,
                         page,
                         offset,
                         sizeof(log_fs_rec_block_t));
    // Match?
    if(memcmp(&log_fs.record_block, &record_block_rd, sizeof(log_fs_rec_block_t)) != 0)
    {
        // Mark record as BAD
        DBG_ERR("Record block write failed (page %u, offset %u)", page, offset);
        log_fs_marker_wr(LOG_FS_MARKER_BAD, page, offset);
        // Failure
        return FALSE;
    }

    // Success
    return TRUE;
}

static log_fs_page_t log_fs_record_pages_bound_start(void)
{
    log_fs_page_t file_page = log_fs.file_info.file.start_page;

    // First page after file page
    return log_fs_page_next(file_page);
}

static log_fs_page_t log_fs_record_pages_bound_end(void)
{
    log_fs_page_t file_page = log_fs.file_info.file.start_page;

#if (LOG_FS_CFG_MAX_PAGES != 0)
    file_page += LOG_FS_CFG_MAX_PAGES;
    // Wrap?
    if(file_page > LOG_FS_CFG_PAGE_END)
    {
        file_page -= (LOG_FS_CFG_PAGE_END - LOG_FS_CFG_PAGE_START);
    }
    return file_page;
#else
    return log_fs_page_previous(file_page);
#endif    
}

static log_fs_page_t log_fs_page_header_find_first(log_fs_marker_t marker,
                                                   log_fs_page_t   page_start, 
                                                   log_fs_page_t   page_end)
{
    // Start at first page
    log_fs_page_t page = page_start;

    // Sanity checks
    DBG_ASSERT((page_start >= LOG_FS_CFG_PAGE_START) && (page_start <= LOG_FS_CFG_PAGE_END));
    DBG_ASSERT((page_end   >= LOG_FS_CFG_PAGE_START) && (page_end   <= LOG_FS_CFG_PAGE_END));

    while(TRUE)
    {
        // Read page header. Does it match specified marker?
        if(log_fs_page_header_rd(page) == marker)
        {
            // Marker found
            return page;
        }
        // Last page read?
        if(page == page_end)
        {
            // Marker not found
            return LOG_FS_PAGE_INVALID;
        }
        // Next page
        page = log_fs_page_next(page);
    }
}

static log_fs_page_t log_fs_page_header_find_next(log_fs_marker_t marker,
                                                  log_fs_page_t   page_start, 
                                                  log_fs_page_t   page_end,
                                                  log_fs_page_t   page_current)
{
    // Start at current page
    log_fs_page_t page = page_current;

    // Sanity checks
    DBG_ASSERT((page_start   >= LOG_FS_CFG_PAGE_START) && (page_start   <= LOG_FS_CFG_PAGE_END));
    DBG_ASSERT((page_end     >= LOG_FS_CFG_PAGE_START) && (page_end     <= LOG_FS_CFG_PAGE_END));
    DBG_ASSERT((page_current >= LOG_FS_CFG_PAGE_START) && (page_current <= LOG_FS_CFG_PAGE_END));

    while(TRUE)
    {
        // End page?
        if(page == page_end)
        {
            // Wrap to start page
            page = page_start;
        }
        else
        {
            // Next page
            page = log_fs_page_next(page);
        }

        // Wrapped all the way around and now back at current page?
        if(page == page_current)
        {
            // Marker not found
            return LOG_FS_PAGE_INVALID;
        }
        
        // Read page header. Does it match specified marker?
        if(log_fs_page_header_rd(page) == marker)
        {
            // Marker found
            return page;
        }
    }
}                        

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
log_fs_err_t log_fs_init(void)
{
    log_fs_page_t file_page_first;
    log_fs_page_t file_page;
    log_fs_page_t file_page_next;

    log_fs_nr_t   file_nr;
    log_fs_nr_t   file_nr_next;
    log_fs_nr_t   file_nr_diff;
    log_fs_nr_t   file_nr_diff_largest;

    // Report structure sizes so that correct packing of structures can be verified
    DBG_INFO("Page header size: %u",  sizeof(log_fs_page_header_t));
    DBG_INFO("File block size: %u",   sizeof(log_fs_file_info_t));
    DBG_INFO("Record block size: %u", sizeof(log_fs_rec_block_t));
    DBG_INFO("Record data size: %u",  LOG_FS_CFG_REC_DATA_SIZE);
    DBG_INFO("Records per page: %u",  LOG_FS_REC_PAGE_DATA_SIZE / sizeof(log_fs_rec_block_t));

    // Sanity checks
    DBG_ASSERT(sizeof(log_fs_rec_block_t) <= LOG_FS_REC_PAGE_DATA_SIZE);    
    if(LOG_FS_REC_PAGE_DATA_SIZE % sizeof(log_fs_rec_block_t) != 0)
    {
        DBG_WARN("%u bytes will be wasted per page", 
                 LOG_FS_REC_PAGE_DATA_SIZE % sizeof(log_fs_rec_block_t));
    }

    // Reset status
    memset(&log_fs, 0, sizeof(log_fs_t));
    
    // Find first page containing a file entry
    file_page_first = log_fs_page_header_find_first(LOG_FS_MARKER_FILE,
                                                    LOG_FS_CFG_PAGE_START,
                                                    LOG_FS_CFG_PAGE_END);


    // No files found?
    if(file_page_first == LOG_FS_PAGE_INVALID)
    {
        DBG_INFO("No files found");
        log_fs.file_page_first = LOG_FS_PAGE_INVALID;
        log_fs.file_page_last  = LOG_FS_PAGE_INVALID;
        return LOG_FS_ERR_NONE;
    }    

    // Reset variable to keep track of largest file number difference
    file_nr_diff_largest = 0;

    // Continue from first page found...
    file_page = file_page_first;
    do
    {
        // Note file number of current file entry 
        file_nr = log_fs.page_header.nr;

        // Find next page containing a file entry
        file_page_next = log_fs_page_header_find_next(LOG_FS_MARKER_FILE,
                                                      LOG_FS_CFG_PAGE_START,
                                                      LOG_FS_CFG_PAGE_END,
                                                      file_page);
        // No other pages with file entries?
        if(file_page_next == LOG_FS_PAGE_INVALID)
        {
            // Only one file found
            DBG_INFO("One file at page %u (last nr %u)", file_page, file_nr);
            log_fs.file_page_first   = file_page;
            log_fs.file_page_last    = file_page;
            log_fs.file_page_nr_next = ++file_nr;
            return LOG_FS_ERR_NONE;
        }

        // Note file number of next file entry
        file_nr_next = log_fs.page_header.nr;

        // Calculate file number difference
        file_nr_diff = file_nr_next - file_nr;

        // Largest file number difference so far?
        if(file_nr_diff_largest < file_nr_diff)
        {
            DBG_INFO("Diff=%u, file_page=%u(nr=%u) ,file_page_next=%u (nr=%u)",
                     file_nr_diff,
                     file_page,      file_nr,
                     file_page_next, file_nr_next);

            // Note largest difference so far
            file_nr_diff_largest = file_nr_diff;
            // Save candidates for first and last file
            log_fs.file_page_first   = file_page_next;
            log_fs.file_page_last    = file_page;
            log_fs.file_page_nr_next = ++file_nr;
        }

        // Next comparison
        file_page = file_page_next;
    }
    while(file_page != file_page_first); // Full circle?

    // Report first and last file
    DBG_INFO("First file at page %u", log_fs.file_page_first);
    DBG_INFO("Last file at page %u", log_fs.file_page_last);
    DBG_INFO("Next file nr %u", log_fs.file_page_nr_next);

    return LOG_FS_ERR_NONE;
}

log_fs_err_t log_fs_create(const log_fs_time_stamp_t * time_stamp)
{
    log_fs_marker_t marker;
    log_fs_page_t   page;
    log_fs_page_t   page_last_record;
    log_fs_page_t   page_new_file;

    // Any existing file in the file system?
    if(log_fs.file_page_last != LOG_FS_PAGE_INVALID)
    {
        // Find last record page after file page
        page = log_fs.file_page_last;
        page_last_record = page;
        do
        {
            // Next page
            page = log_fs_page_next(page);
            // Read page header
            marker = log_fs_page_header_rd(page);
            // Valid record page?
            if(marker == LOG_FS_MARKER_RECORD)
            {
                // Remember last record page
                page_last_record = page;
            }
        }
        while(marker != LOG_FS_MARKER_FILE);

        // Next page after last record page is potentially available
        page_new_file = log_fs_page_next(page_last_record);

        // Is this page a file marker?
        if(log_fs_marker_rd(page_new_file, 0) == LOG_FS_MARKER_FILE)
        {
            // Yes, file system is full
            DBG_WARN("File system is full");
            return LOG_FS_ERR_FULL;
        }

        // At least one more page available for records?
        if(log_fs_marker_rd(log_fs_page_next(page_new_file), 0) == LOG_FS_MARKER_FILE)
        {
            // No, file system is full
            DBG_WARN("File system is full");
            return LOG_FS_ERR_FULL;
        }
    }
    else
    {
        // Find first free page
        page = LOG_FS_CFG_PAGE_START;
        do
        {
            // FREE marker?
            if(log_fs_marker_rd(page, 0) == LOG_FS_MARKER_FREE)
            {
                // This page will be used to create a new file
                page_new_file = page;
                break;
            }
            // Next page
            page++;
        }
        while(page <= LOG_FS_CFG_PAGE_END);

        // No FREE markers found?
        if(page > LOG_FS_CFG_PAGE_END)
        {
            // Start at first page
            page_new_file = LOG_FS_CFG_PAGE_START;            
        }
    }

    // Erase file page (if not FREE)
    if(log_fs_marker_rd(page_new_file, 0) != LOG_FS_MARKER_FREE)
    {
        DBG_INFO("Erase file page %u", page_new_file);
        at45d_erase_page(page_new_file);
    }

    // Populate file info
    log_fs.page_header.nr = log_fs.file_page_nr_next;
    memcpy(&log_fs.file_info.file.time_stamp, 
           time_stamp,
           sizeof(log_fs_time_stamp_t));

    // Write file page
    if(!log_fs_file_block_wr(page_new_file))
    {
        DBG_WARN("Write failed");
        return LOG_FS_ERR_WRITE_FAIL;
    }
    DBG_INFO("New file created at page %u (nr %u)", page_new_file, log_fs.page_header.nr);

    // Remember last file
    log_fs.file_page_last = page_new_file;
    // Is this also the first file?
    if(log_fs.file_page_first == LOG_FS_PAGE_INVALID)
    {
        // Remember first file
        log_fs.file_page_first = page_new_file;
    }
    // Remember next file number
    log_fs.file_page_nr_next++;

    // Erase first record page (if not FREE)
    page = log_fs_page_next(page_new_file);
    if(log_fs_marker_rd(page, 0) != LOG_FS_MARKER_FREE)
    {
        DBG_INFO("Erase first record page %u", page);
        at45d_erase_page(page);
    }

    // Reset read and write addresses
    log_fs.rec_page_first    = LOG_FS_PAGE_INVALID;
    log_fs.rec_page_last     = LOG_FS_PAGE_INVALID;
    log_fs.rec_page_nr_next  = 0;
    log_fs.rec_adr_rd.page   = LOG_FS_PAGE_INVALID;
    log_fs.rec_adr_wr.page   = page;
    log_fs.rec_adr_wr.offset = LOG_FS_REC_OFFSET_FIRST;

    return LOG_FS_ERR_NONE;
}

size_t log_fs_file_attr_data_wr(log_fs_offset_t offset, 
                                const void *    data, 
                                size_t          nr_of_bytes)
{
    // Does offset exceed page size?
    if(offset >= LOG_FS_FILE_ATTR_DATA_SIZE)
    {
        // No bytes could be written
        return 0;
    }
    // Does number of bytes exceed page size?
    if(nr_of_bytes > (LOG_FS_FILE_ATTR_DATA_SIZE - offset))
    {
        // Clip number of bytes
        nr_of_bytes = LOG_FS_FILE_ATTR_DATA_SIZE - offset;
    }
    // Write file attribute data
    at45d_wr_page_offset(data, 
                         log_fs.file_info.file.start_page, 
                         sizeof(log_fs_page_header_t) + sizeof(log_fs_file_info_t) + offset,
                         nr_of_bytes);

    return nr_of_bytes;
}

size_t log_fs_file_attr_data_rd(log_fs_offset_t offset, 
                                void *          data, 
                                size_t          nr_of_bytes)
{
    // Does offset exceed page size?
    if(offset >= LOG_FS_FILE_ATTR_DATA_SIZE)
    {
        // No bytes could be read
        return 0;
    }
    // Does number of bytes exceed page size?
    if(nr_of_bytes > (LOG_FS_FILE_ATTR_DATA_SIZE - offset))
    {
        // Clip number of bytes
        nr_of_bytes = LOG_FS_FILE_ATTR_DATA_SIZE - offset;
    }
    // Write file attribute data
    at45d_rd_page_offset(data, 
                         log_fs.file_info.file.start_page, 
                         sizeof(log_fs_page_header_t) + sizeof(log_fs_file_info_t) + offset,
                         nr_of_bytes);

    return nr_of_bytes;
}

size_t log_fs_file_attr_data_size(void)
{
    return LOG_FS_FILE_ATTR_DATA_SIZE;
}

log_fs_err_t log_fs_file_find_first(log_fs_file_t * file)
{
    // Any files?
    if(log_fs.file_page_first == LOG_FS_PAGE_INVALID)
    {
        // No files
        return LOG_FS_ERR_NO_FILE;
    }

    // Read file info
    if(!log_fs_file_block_rd(log_fs.file_page_first))
    {
        // Invalid file info
        DBG_ERR("File info was valid during log_fs_init()");
        return LOG_FS_ERR_FILE_INVALID;
    }

    // Return file info
    memcpy(file, &log_fs.file_info.file, sizeof(log_fs_file_t));
    DBG_INFO("First file nr %u found at page %u", log_fs.page_header.nr, log_fs.file_info.file.start_page);
    return LOG_FS_ERR_NONE;
}

log_fs_err_t log_fs_file_find_last(log_fs_file_t * file)
{
    // Any files?
    if(log_fs.file_page_last == LOG_FS_PAGE_INVALID)
    {
        // No files
        return LOG_FS_ERR_NO_FILE;
    }

    // Read file info
    if(!log_fs_file_block_rd(log_fs.file_page_last))
    {
        DBG_ERR("File info was valid during log_fs_init()");
        return LOG_FS_ERR_FILE_INVALID;
    }

    // Return file info
    memcpy(file, &log_fs.file_info.file, sizeof(log_fs_file_t));
    DBG_INFO("Last file nr %u found at page %u", log_fs.page_header.nr, log_fs.file_info.file.start_page);
    return LOG_FS_ERR_NONE;
}

log_fs_err_t log_fs_file_find_next(log_fs_file_t * file)
{
    log_fs_page_t page;

    // Any files?
    if(log_fs.file_page_last == LOG_FS_PAGE_INVALID)
    {
        // No files
        return LOG_FS_ERR_NO_FILE;
    }

    // Save page of current file block
    page = log_fs.file_info.file.start_page;

    // Was current file the last?
    if(page == log_fs.file_page_last)
    {
        // No next file
        return LOG_FS_ERR_NO_FILE;
    }

    // Start with current file page
    do
    {
        // Next page
        page = log_fs_page_next(page);        

        // Read (possible) file entry at start of page
        if(log_fs_file_block_rd(page))
        {
            // Return file info
            memcpy(file, &log_fs.file_info.file, sizeof(log_fs_file_t));
            DBG_INFO("Next file nr %u found at page %u", log_fs.page_header.nr, log_fs.file_info.file.start_page);
            return LOG_FS_ERR_NONE;
        }
    }
    while(page != log_fs.file_page_last);

    // No file entry found
    return LOG_FS_ERR_NO_FILE;
}

log_fs_err_t log_fs_file_find_previous(log_fs_file_t * file)
{
    log_fs_page_t page;

    // Any files?
    if(log_fs.file_page_first == LOG_FS_PAGE_INVALID)
    {
        // No files
        return LOG_FS_ERR_NO_FILE;
    }

    // Save page of current file block
    page = log_fs.file_info.file.start_page;

    // Was current file the first?
    if(page == log_fs.file_page_first)
    {
        // No next file
        return LOG_FS_ERR_NO_FILE;
    }

    // Start with current file page
    do
    {
        // Previous page
        page = log_fs_page_previous(page);

        // Read (possible) file entry at start of page
        if(log_fs_file_block_rd(page))
        {
            // Return file info
            memcpy(file, &log_fs.file_info.file, sizeof(log_fs_file_t));
            DBG_INFO("Previous file nr %u found at page %u", log_fs.page_header.nr, log_fs.file_info.file.start_page);
            return LOG_FS_ERR_NONE;
        }
    }
    while(page != log_fs.file_page_first);

    // No file entry found
    return LOG_FS_ERR_NO_FILE;
}

log_fs_err_t log_fs_open(const log_fs_file_t * file)
{
    log_fs_page_t   rec_search_page_start;
    log_fs_page_t   rec_search_page_end;

    log_fs_page_t   rec_page_first;
    log_fs_page_t   rec_page;
    log_fs_page_t   rec_page_next;

    log_fs_nr_t     rec_nr;
    log_fs_nr_t     rec_nr_next;
    log_fs_nr_t     rec_nr_diff;
    log_fs_nr_t     rec_nr_diff_largest;

    log_fs_marker_t marker;
    log_fs_offset_t offset;
    
    // Sanity check
    if(file == NULL)
    {
        DBG_ERR("Invalid file");
        return LOG_FS_ERR_FILE_INVALID;
    }

    // Read file block (again)
    DBG_INFO("File start page=%u", file->start_page);
    if(!log_fs_file_block_rd(file->start_page))
    {
        DBG_ERR("Invalid file");
        return LOG_FS_ERR_FILE_INVALID;
    }    

    // Start search for record pages after file page
    rec_search_page_start = log_fs_record_pages_bound_start();

    // Two consecutive file pages?
    if(log_fs_page_header_rd(rec_search_page_start) == LOG_FS_MARKER_FILE)
    {
        // File is empty
        DBG_INFO("File is empty");
        log_fs.rec_page_first    = LOG_FS_PAGE_INVALID;
        log_fs.rec_page_last     = LOG_FS_PAGE_INVALID;
        log_fs.rec_page_nr_next  = 0;
        log_fs.rec_adr_rd.page   = LOG_FS_PAGE_INVALID;
        log_fs.rec_adr_wr.page   = rec_search_page_start;
        log_fs.rec_adr_wr.offset = LOG_FS_REC_OFFSET_FIRST;
        return LOG_FS_ERR_NONE;
    }

    // End search for record pages before file page (or boundary if LOG_FS_CFG_MAX_PAGES != 0)
    rec_search_page_end = log_fs_record_pages_bound_end();

    // Is there more than one file in the file system?
    if(log_fs.file_page_first != log_fs.file_page_last)
    {
        // Find next file page
        rec_search_page_end = log_fs_page_header_find_first(LOG_FS_MARKER_FILE,
                                                            rec_search_page_start,
                                                            log_fs.file_info.file.start_page);
        // Two consecutive file pages?
        if(rec_search_page_end == rec_search_page_start)
        {
            // File is empty
            DBG_INFO("File is empty");
            log_fs.rec_page_first    = LOG_FS_PAGE_INVALID;
            log_fs.rec_page_last     = LOG_FS_PAGE_INVALID;
            log_fs.rec_page_nr_next  = 0;
            log_fs.rec_adr_rd.page   = LOG_FS_PAGE_INVALID;
            log_fs.rec_adr_wr.page   = rec_search_page_start;
            log_fs.rec_adr_wr.offset = LOG_FS_REC_OFFSET_FIRST;
            return LOG_FS_ERR_NONE;
        }
        // Next file page found?
        if(rec_search_page_end != LOG_FS_PAGE_INVALID)
        {
            // End search just before next file page
            rec_search_page_end = log_fs_page_previous(rec_search_page_end);
            
        }
        else
        {
            // End search for record pages before file page (or boundary if LOG_FS_CFG_MAX_PAGES != 0)
            rec_search_page_end = log_fs_record_pages_bound_end();
        }        
    }
    DBG_INFO("rec_search_page_start=%u", rec_search_page_start);
    DBG_INFO("rec_search_page_end=%u",   rec_search_page_end);

    // Find first valid record page
    rec_page_first = log_fs_page_header_find_first(LOG_FS_MARKER_RECORD,
                                                   rec_search_page_start,
                                                   rec_search_page_end);


    // No record pages found?
    if(rec_page_first == LOG_FS_PAGE_INVALID)
    {
        DBG_INFO("No record pages found");
        log_fs.rec_page_first    = LOG_FS_PAGE_INVALID;
        log_fs.rec_page_last     = LOG_FS_PAGE_INVALID;
        log_fs.rec_page_nr_next  = 0;
        log_fs.rec_adr_rd.page   = LOG_FS_PAGE_INVALID;
        log_fs.rec_adr_wr.page   = rec_search_page_start;
        log_fs.rec_adr_wr.offset = LOG_FS_REC_OFFSET_FIRST;

        // First record page not FREE?
        if(log_fs_marker_rd(rec_search_page_start, 0) != LOG_FS_MARKER_FREE)
        {
            // Erase first record page
            DBG_INFO("Erase first record page %u", rec_search_page_start);
            at45d_erase_page(rec_search_page_start);
        }

        return LOG_FS_ERR_NONE;
    }    

    // Reset variable to keep track of largest record number difference
    rec_nr_diff_largest = 0;

    // Continue from first page found...
    rec_page = rec_page_first;
    do
    {
        // Note record number of current record page entry
        rec_nr = log_fs.page_header.nr;

        // Find next page containing a record page entry
        rec_page_next = log_fs_page_header_find_next(LOG_FS_MARKER_RECORD,
                                                     rec_search_page_start,
                                                     rec_search_page_end,
                                                     rec_page);
        // No other pages with record page entries?
        if(rec_page_next == LOG_FS_PAGE_INVALID)
        {
            // Only one record page found
            DBG_INFO("One record page %u (last nr %u)", rec_page, rec_nr);
            log_fs.rec_page_first    = rec_page;
            log_fs.rec_page_last     = rec_page;
            log_fs.rec_page_nr_next  = ++rec_nr;
            break;
        }

        // Note record number of next record page entry
        rec_nr_next = log_fs.page_header.nr;

        // Calculate record number difference
        rec_nr_diff = rec_nr_next - rec_nr;

        // Largest record number difference so far?
        if(rec_nr_diff_largest < rec_nr_diff)
        {
            DBG_INFO("Diff=%u, rec_page=%u(nr=%u) ,rec_page_next=%u (nr=%u)",
                     rec_nr_diff,
                     rec_page,      rec_nr,
                     rec_page_next, rec_nr_next);

            // Note largest difference so far
            rec_nr_diff_largest = rec_nr_diff;
            // Save candidates for first and last record page
            log_fs.rec_page_first    = rec_page_next;
            log_fs.rec_page_last     = rec_page;
            log_fs.rec_page_nr_next  = ++rec_nr;
        }

        // Next comparison
        rec_page = rec_page_next;
    }
    while(rec_page != rec_page_first); // Full circle?

    DBG_INFO("rec_page_first = %u",    log_fs.rec_page_first);
    DBG_INFO("rec_page_last = %u",     log_fs.rec_page_last);
    DBG_INFO("rec_page_nr_next = %u",  log_fs.rec_page_nr_next);

    // Find next free record position
    rec_page = log_fs.rec_page_last;
    offset   = LOG_FS_REC_OFFSET_FIRST;
    while(offset <= LOG_FS_REC_OFFSET_LAST)
    {
        marker = log_fs_marker_rd(rec_page, offset);
        if(marker == LOG_FS_MARKER_FREE)
        {
            break;
        }
        offset += sizeof(log_fs_rec_block_t);
    }

    // Free space found?
    if(marker == LOG_FS_MARKER_FREE)
    {
        // Save location of first free record entry
        log_fs.rec_adr_wr.page   = rec_page;
        log_fs.rec_adr_wr.offset = offset;
    }
    else
    {
        // Last record page is full
        DBG_INFO("Last rec page (%u) is full", rec_page);
        // Next page
        rec_page = log_fs_page_next(rec_page);

#if (LOG_FS_CFG_TYPE == LOG_FS_CFG_TYPE_LINEAR)
        // Space available?
        if(rec_page != log_fs_page_next(log_fs_record_pages_bound_end()))
        {
            marker = log_fs_marker_rd(rec_page, 0);
            if(marker == LOG_FS_MARKER_FILE)
            {
                DBG_INFO("File is full. More data cannot be appended");
            }
            else if(marker != LOG_FS_MARKER_FREE)
            {
                // Erase next page
                DBG_INFO("Erase page %u", rec_page);
                at45d_erase_page(rec_page);
            }
        }
#endif

#if (LOG_FS_CFG_TYPE == LOG_FS_CFG_TYPE_CIRCULAR)
        // Maximum space reached?
        if(  (rec_page == log_fs_page_next(log_fs_record_pages_bound_end()))
           ||(log_fs_marker_rd(rec_page, 0) == LOG_FS_MARKER_FILE          )  )
        {
            // Wrap
            rec_page = log_fs_record_pages_bound_start();
        }
        // Erase next page
        DBG_INFO("Erase page %u", rec_page);
        at45d_erase_page(rec_page);
        // First page erased?
        if(rec_page == log_fs.rec_page_first)
        {
            // Find next page containing a record page entry
            log_fs.rec_page_first = log_fs_page_header_find_next(LOG_FS_MARKER_RECORD,
                                                                 rec_search_page_start,
                                                                 rec_search_page_end,
                                                                 rec_page);
        }
#endif

        // Next record write address
        log_fs.rec_adr_wr.page   = rec_page;
        log_fs.rec_adr_wr.offset = LOG_FS_REC_OFFSET_FIRST;
    }

    DBG_INFO("rec_adr_wr.page=%u",   log_fs.rec_adr_wr.page);
    DBG_INFO("rec_adr_wr.offset=%u", log_fs.rec_adr_wr.offset);

    return LOG_FS_ERR_NONE;
}

log_fs_err_t log_fs_file_delete(const log_fs_file_t * file)
{
    log_fs_marker_t marker;
    log_fs_page_t   page = file->start_page;

    // Not the oldest file?
    if(page != log_fs.file_page_first)
    {
        DBG_ERR("Not the oldest file");
        return LOG_FS_ERR_FILE_INVALID;
    }

    // (Sanity check) valid file page?
    if(!log_fs_file_block_rd(page))
    {
        DBG_ERR("Invalid file");
        return LOG_FS_ERR_FILE_INVALID;
    }

    // Only one file in file system?
    if(log_fs.file_page_first == log_fs.file_page_last)
    {
        DBG_INFO("Deleting last file");
        // No files in file system anymore (after this one is deleted)
        log_fs.file_page_first = LOG_FS_PAGE_INVALID;
        log_fs.file_page_last  = LOG_FS_PAGE_INVALID;
    }
    else
    {
        // Start before next file page
        page = log_fs_page_header_find_next(LOG_FS_MARKER_FILE,
                                            log_fs.file_page_first,
                                            log_fs.file_page_last,
                                            log_fs.file_page_first);

        // Next file will be the first (after this one is deleted)
        log_fs.file_page_first = page;
        DBG_INFO("First file will be at page %u", log_fs.file_page_first);
    }

    // Reverse delete all records
    do
    {
        // Previous page
        page = log_fs_page_previous(page);

        // Read page header
        marker = log_fs_page_header_rd(page);

        if(marker == LOG_FS_MARKER_RECORD)
        {
            // Mark RECORD page as BAD
            DBG_INFO("Marking RECORD page %u as BAD", page);
            log_fs_marker_wr(LOG_FS_MARKER_BAD, page, 0);
        }
    }
    while(marker != LOG_FS_MARKER_FILE);
    
    // Mark FILE page as BAD
    DBG_INFO("Marking FILE page %u as BAD", page);
    log_fs_marker_wr(LOG_FS_MARKER_BAD, page, 0);

    return LOG_FS_ERR_NONE;
}

log_fs_err_t log_fs_record_rd_first(void * data, size_t nr_of_bytes)
{
    // No record pages?
    if(log_fs.rec_page_first == LOG_FS_PAGE_INVALID)
    {
        return LOG_FS_ERR_NO_RECORD;
    }

    // Start before first record
    log_fs.rec_adr_rd.page   = log_fs_page_previous(log_fs.rec_page_first);
    log_fs.rec_adr_rd.offset = LOG_FS_REC_OFFSET_LAST;    

    // Return first valid record
    return log_fs_record_rd_next(data, nr_of_bytes);
}

log_fs_err_t log_fs_record_rd_last(void * data, size_t nr_of_bytes)
{
    // No record pages?
    if(log_fs.rec_page_last == LOG_FS_PAGE_INVALID)
    {
        return LOG_FS_ERR_NO_RECORD;
    }

    // Start at free position
    log_fs.rec_adr_rd.page   = log_fs.rec_adr_wr.page;
    log_fs.rec_adr_rd.offset = log_fs.rec_adr_wr.offset;
    
    // Return first valid record
    return log_fs_record_rd_previous(data, nr_of_bytes);
}

log_fs_err_t log_fs_record_rd_next(void * data, size_t nr_of_bytes)
{
    log_fs_marker_t marker;
    log_fs_page_t   page   = log_fs.rec_adr_rd.page;
    log_fs_page_t   offset = log_fs.rec_adr_rd.offset;

    // More bytes requested than can be stored in record?
    if(nr_of_bytes > LOG_FS_CFG_REC_DATA_SIZE)
    {
        DBG_ERR("More bytes requested than can be stored in a record");
        // Clip number of bytes that will be copied
        nr_of_bytes = LOG_FS_CFG_REC_DATA_SIZE;
    }

    // Find next valid record
    while(TRUE)
    {
        // End of page?
        if(offset >= LOG_FS_REC_OFFSET_LAST)
        {
            // Start record reading after page header
            offset = LOG_FS_REC_OFFSET_FIRST;
            
            // Next record page
            do
            {

#if (LOG_FS_CFG_TYPE == LOG_FS_CFG_TYPE_LINEAR)
                // Next page
                page = log_fs_page_next(page);
                // Wrapped completely?
                if(page == log_fs.rec_adr_rd.page)
                {
                    // Should have stopped at file marker, but it was not found
                    DBG_ERR("Valid file marker not found");
                    log_fs.rec_adr_rd.page = LOG_FS_PAGE_INVALID;
                    // Stop
                    return LOG_FS_ERR_NO_RECORD;
                }
#endif

#if (LOG_FS_CFG_TYPE == LOG_FS_CFG_TYPE_CIRCULAR)
                // End page reached?
                if(page == log_fs_record_pages_bound_end())
                { 
                    // Wrap to start page
                    page = log_fs_record_pages_bound_start();
                }
                else
                {
                    // Next page
                    page = log_fs_page_next(page);
                }
                // Has last record been read?
                if(  (page   == log_fs.rec_adr_wr.page  )
                   &&(offset == log_fs.rec_adr_wr.offset)  )
                {
                    // Stop
                    return LOG_FS_ERR_NO_RECORD;
                }
#endif

                // Read page header
                marker = log_fs_page_header_rd(page);

#if (LOG_FS_CFG_TYPE == LOG_FS_CFG_TYPE_LINEAR)
                // Next file reached?
                if(marker == LOG_FS_MARKER_FILE)
                {
                    // Stop
                    return LOG_FS_ERR_NO_RECORD;
                }
#endif

            }
            while(marker != LOG_FS_MARKER_RECORD);
        }
        else
        {
            // Next record address
            offset += sizeof(log_fs_rec_block_t);        
        }

        // Has last record been read?
        if(  (page   == log_fs.rec_adr_wr.page  )
           &&(offset == log_fs.rec_adr_wr.offset)  )
        {
            // Stop
            return LOG_FS_ERR_NO_RECORD;
        }

        // Read record
        if(log_fs_record_block_rd(page, offset))
        {
            // Next valid record found
            log_fs.rec_adr_rd.page   = page;
            log_fs.rec_adr_rd.offset = offset;
            // Copy content of record to user supplied buffer
            memcpy(data, &log_fs.record_block.data, nr_of_bytes);
            // Valid record
            return LOG_FS_ERR_NONE;
        }
    }
}

log_fs_err_t log_fs_record_rd_previous(void * data, size_t nr_of_bytes)
{
    log_fs_marker_t marker;
    log_fs_page_t   page   = log_fs.rec_adr_rd.page;
    log_fs_page_t   offset = log_fs.rec_adr_rd.offset;

    // More bytes requested than can be stored in record?
    if(nr_of_bytes > LOG_FS_CFG_REC_DATA_SIZE)
    {
        DBG_ERR("More bytes requested than can be stored in a record");
        // Clip number of bytes that will be copied
        nr_of_bytes = LOG_FS_CFG_REC_DATA_SIZE;
    }
    
    // Find previous valid record
    while(TRUE)
    {
        // End of page?
        if(offset <= LOG_FS_REC_OFFSET_FIRST)
        {
            // Start record reading at last offset
            offset = LOG_FS_REC_OFFSET_LAST;
            
            // Previous record page
            do
            {
#if (LOG_FS_CFG_TYPE == LOG_FS_CFG_TYPE_LINEAR)
                // Previous page
                page = log_fs_page_previous(page);
                // Has first record been read?
                if(page == log_fs.file_info.file.start_page)
                {
                    // Stop
                    return LOG_FS_ERR_NO_RECORD;
                }
#endif
#if (LOG_FS_CFG_TYPE == LOG_FS_CFG_TYPE_CIRCULAR)
                // Start page reached?
                if(page == log_fs_record_pages_bound_start())
                { 
                    // Wrap to end page
                    page = log_fs_record_pages_bound_end();
                }
                else
                {
                    // Previous page
                    page = log_fs_page_previous(page);
                }
                // Has first record been read?
                if(page == log_fs.rec_adr_wr.page)
                {
                    // Stop
                    return LOG_FS_ERR_NO_RECORD;
                }
#endif
                // Read page header
                marker = log_fs_page_header_rd(page);
            }
            while(marker != LOG_FS_MARKER_RECORD);            
        }
        else
        {
            // Previous record address
            offset -= sizeof(log_fs_rec_block_t);        
        }

        // Read record
        if(log_fs_record_block_rd(page, offset))
        {
            // Next valid record found
            log_fs.rec_adr_rd.page   = page;
            log_fs.rec_adr_rd.offset = offset;
            // Copy content of record to user supplied buffer
            memcpy(data, &log_fs.record_block.data, nr_of_bytes);
            // Valid record
            return LOG_FS_ERR_NONE;
        }
    }
}

log_fs_err_t log_fs_record_wr(const void * data, size_t nr_of_bytes)
{
    size_t          i;
    log_fs_marker_t marker;
    log_fs_page_t   page    = log_fs.rec_adr_wr.page;
    log_fs_page_t   offset  = log_fs.rec_adr_wr.offset;
    u8_t *          data_u8 = (u8_t *)data;
    bool_t          success;

    // Record too small to hold all of the supplied data?
    if(nr_of_bytes > LOG_FS_CFG_REC_DATA_SIZE)
    {
        DBG_ERR("Record size too small and %u bytes will be discarded",
                (nr_of_bytes - LOG_FS_CFG_REC_DATA_SIZE));
    }

    // First record to write in page?
    if(offset == LOG_FS_REC_OFFSET_FIRST)
    {
        // Read page's marker
        marker = log_fs_marker_rd(page, 0);
 
#if (LOG_FS_CFG_TYPE == LOG_FS_CFG_TYPE_LINEAR)
        // Maximum space reached?
        if(  (marker == LOG_FS_MARKER_FILE                               )
           ||(page   == log_fs_page_next(log_fs_record_pages_bound_end()))  )
        {
            // File system full
            DBG_WARN("File system is full");
            return LOG_FS_ERR_FULL;
        }
#endif

        // Page marked as BAD?
        if(marker == LOG_FS_MARKER_BAD)
        {
            DBG_INFO("Erase page %u", page);
            at45d_erase_page(page);
        }

        // Write record page header
        log_fs.page_header.marker  = LOG_FS_MARKER_RECORD;
        log_fs.page_header.nr      = log_fs.rec_page_nr_next;
        if(!log_fs_page_header_wr(page))
        {
            // Failed to write record page header. Move on to next page
            page = log_fs_page_next(page);

            // Read next page's marker
            marker = log_fs_marker_rd(page, 0);

            // Maximum space reached?
            if(  (marker == LOG_FS_MARKER_FILE                               )
               ||(page   == log_fs_page_next(log_fs_record_pages_bound_end()))  )
            {
#if (LOG_FS_CFG_TYPE == LOG_FS_CFG_TYPE_LINEAR)
                // File system full
                DBG_WARN("File system is full");
                return LOG_FS_ERR_FULL;
#endif

#if (LOG_FS_CFG_TYPE == LOG_FS_CFG_TYPE_CIRCULAR)
                // Wrap
                page = log_fs_record_pages_bound_start();
#endif
            }

            // Erase next page
            DBG_INFO("Erase page %u", page);
            at45d_erase_page(page);
            // Next record write page (record number is not incremented)
            log_fs.rec_adr_wr.page = page;

#if (LOG_FS_CFG_TYPE == LOG_FS_CFG_TYPE_CIRCULAR)
            // First record page erased?
            if(page == log_fs.rec_page_first)
            {
                // Next record page will be the first
                log_fs.rec_page_first = log_fs_page_header_find_next(LOG_FS_MARKER_RECORD,
                                                                     log_fs_record_pages_bound_start(),
                                                                     log_fs_record_pages_bound_end(),
                                                                     log_fs.rec_page_first);
                DBG_INFO("rec_page_first=%u", log_fs.rec_page_first);
            }
#endif
            // Failed to write record page header correctly
            DBG_WARN("Write failed");
            return LOG_FS_ERR_WRITE_FAIL;
        }
        DBG_INFO("Page %u written with RECORD page header (nr=%u)",
                 log_fs.rec_adr_wr.page,
                 log_fs.page_header.nr);
        // Next record number
        log_fs.rec_page_nr_next++;
        DBG_INFO("rec_page_nr_next=%u", log_fs.rec_page_nr_next);

        // Update last record page
        log_fs.rec_page_last = page;
        DBG_INFO("rec_page_last=%u", log_fs.rec_page_last);
        // Is this also the first record page?
        if(log_fs.rec_page_first == LOG_FS_PAGE_INVALID)
        {
            log_fs.rec_page_first = page;
            DBG_INFO("rec_page_first=%u", log_fs.rec_page_first);
        }
        
    }

    // Copy data
    for(i=0; i<LOG_FS_CFG_REC_DATA_SIZE; i++)
    {
        // Byte to copy?
        if(nr_of_bytes != 0)
        {
            nr_of_bytes--;
            log_fs.record_block.data[i] = *data_u8++;
        }
        else
        {
            // Fill rest of array with 0xff
            log_fs.record_block.data[i] = 0xff;
        }
    }    
    
    // Write record
    DBG_INFO("Write record (page %u, offset %u)", page, offset);
    success = log_fs_record_block_wr(page, offset);

    // Last record in page?
    if(offset == LOG_FS_REC_OFFSET_LAST)
    {
        // Next page
        page = log_fs_page_next(page);

#if (LOG_FS_CFG_TYPE == LOG_FS_CFG_TYPE_LINEAR)
        // Space available?
        if(  (page != log_fs_page_next(log_fs_record_pages_bound_end()))
           &&(log_fs_marker_rd(page, 0) != LOG_FS_MARKER_FILE          )  )
        {
            // Erase next page
            DBG_INFO("Erase page %u", page);
            at45d_erase_page(page);
        }
#endif

#if (LOG_FS_CFG_TYPE == LOG_FS_CFG_TYPE_CIRCULAR)
        // Maximum space reached?
        if(  (page == log_fs_page_next(log_fs_record_pages_bound_end()))
           ||(log_fs_marker_rd(page, 0) == LOG_FS_MARKER_FILE          )  )
        {
            // Wrap
            page = log_fs_record_pages_bound_start();
        }
        // Erase next page
        DBG_INFO("Erase page %u", page);
        at45d_erase_page(page);

        // First record page erased?
        if(page == log_fs.rec_page_first)
        {
            // Next record page will be the first
            log_fs.rec_page_first = log_fs_page_header_find_next(LOG_FS_MARKER_RECORD,
                                                                 log_fs_record_pages_bound_start(),
                                                                 log_fs_record_pages_bound_end(),
                                                                 log_fs.rec_page_first);
            DBG_INFO("rec_page_first=%u", log_fs.rec_page_first);
        }
#endif

        // Next record write address
        log_fs.rec_adr_wr.page   = page;
        log_fs.rec_adr_wr.offset = LOG_FS_REC_OFFSET_FIRST;        
    }
    else
    {
        // Next offset
        log_fs.rec_adr_wr.offset = offset + sizeof(log_fs_rec_block_t);
    }
    
    // Success?
    if(success)
    {
        return LOG_FS_ERR_NONE;
    }
    else
    {
        DBG_WARN("Write failed");
        return LOG_FS_ERR_WRITE_FAIL;
    }
}

void log_fs_info(void)
{
    log_fs_page_t page;
    log_fs_marker_t marker;
    u8_t i;

    printf("Page start: 0x%04X\n",    LOG_FS_CFG_PAGE_START);
    printf("Page end: 0x%04X\n",      LOG_FS_CFG_PAGE_END);
    printf("Page size: %u\n",         AT45D_PAGE_SIZE);
    printf("Record size: %u\n",       sizeof(log_fs_rec_block_t));
    printf("Record data size: %u\n",  LOG_FS_CFG_REC_DATA_SIZE);
    printf("Records per page: %u\n",  LOG_FS_REC_PAGE_DATA_SIZE / sizeof(log_fs_rec_block_t));
    printf("First file: %u\n",        log_fs.file_page_first);
    printf("Last file: %u\n",         log_fs.file_page_last);

    printf("\n        ");
    for(i = 0; i < 16; i++)
    {
    	printf("%01X      ", i);
    }
    printf("\n");

    i=0;
    for(page = LOG_FS_CFG_PAGE_START; page<=LOG_FS_CFG_PAGE_END; page++)
    {
        if(i == 0)
        {
        	printf("\n0x%04X: ", page);
        }

        marker = log_fs_page_header_rd(page);
        switch(marker)
        {
        case LOG_FS_MARKER_FREE:
        	printf("x      ");
            break;
        case LOG_FS_MARKER_FILE:
        	printf("F %04x ", log_fs.page_header.nr);
            break;
        case LOG_FS_MARKER_RECORD:
        	printf("R %04x ", log_fs.page_header.nr);
            break;
        case LOG_FS_MARKER_BAD:
            // Fall through...
        default:
        	printf("B      ");
            break;
        }

        if(++i == 16)
        {
            i = 0;
        }
    }
    printf("\n");
}
