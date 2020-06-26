#ifndef __AT45D_H__
#define __AT45D_H__
/* =============================================================================

    Copyright (c) 2010 Pieter Conradie <http://piconomix.com>
 
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

    Title:          at45d.h : Atmel AT45D Serial DataFlash Driver
    Author(s):      Pieter Conradie
    Creation Date:  2010-04-15

============================================================================= */
/** 
    @ingroup DEVICES_MEM
    @defgroup AT45D at45d.h : Atmel AT45D DataFlash Driver

    Driver that communicates with an Atmel AT45D DataFlash using SPI.

    File(s):
    - devices/mem/at45d.h
    - devices/mem/at45d_cfg_template.h
    - devices/mem/at45d.c

    Reference:
    - [Adesto AT45DB041E] (http://www.adestotech.com/sites/default/files/datasheets/doc8783.pdf) 4Mbit DataFlash datasheet
 
    Example:
 
    @include devices/mem/test/at45d_test.c
 
 */
/// @{

/* _____PROJECT INCLUDES_____________________________________________________ */
#include <defines.h>
#include <chip.h>

// Include project specific config. See "at45d_cfg_template.h"
#include <data_Manager/at45d_cfg.h>

// Check that all project specific options have been specified in "at45d_cfg.h"
#ifndef AT45D_CFG_DEVICE
#error "AT45D_CFG_DEVICE not specified"
#endif
#ifndef AT45D_CFG_PWR_OF_TWO_PAGE_SIZE
#error "AT45D_CFG_PWR_OF_TWO_PAGE_SIZE not specified"
#endif

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS _________________________________________________________ */
/// @name List AT45D devices
//@{
#define AT45DB011 0 /// 1M bit, 2.7-Volt Minimum Serial-Interface Flash with One 264-Byte SRAM Buffer
#define AT45DB021 1 /// 2M bit, 2.7-Volt Minimum Serial-Interface Flash with One 264-Byte SRAM Buffer
#define AT45DB041 2 /// 4M bit 2.5-Volt or 2.7-Volt DataFlash
#define AT45DB081 3 /// 8M bit, 2.5 or 2.7-Volt Only Serial-Interface Flash
#define AT45DB161 4 /// 16M bit, 2.7-Volt Only Serial-Interface Flash with two SRAM Data Buffers
#define AT45DB321 5 /// 32M bit, 2.7-Volt Only Serial Interface Flash
#define AT45DB642 6 /// 64M bit, 2.7-Volt Dual-Interface Flash with two 1056-Byte SRAM
//@}

// Determine number of pages and page size according to device specified
#if   (AT45D_CFG_DEVICE == AT45DB011)
#define AT45D_PAGES             512ul
#define AT45D_PAGE_SIZE         (256 + (AT45D_CFG_PWR_OF_TWO_PAGE_SIZE==0 ? 8 : 0))
#elif (AT45D_CFG_DEVICE == AT45DB021)
#define AT45D_PAGES             1024ul
#define AT45D_PAGE_SIZE         (256 + (AT45D_CFG_PWR_OF_TWO_PAGE_SIZE==0 ? 8 : 0))
#elif (AT45D_CFG_DEVICE == AT45DB041)
#define AT45D_PAGES             2048ul
#define AT45D_PAGE_SIZE         (256 + (AT45D_CFG_PWR_OF_TWO_PAGE_SIZE==0 ? 8 : 0))
#define AT45D_DENSITY           0x1C
#elif (AT45D_CFG_DEVICE == AT45DB081)
#define AT45D_PAGES             4096ul
#define AT45D_PAGE_SIZE         (256 + (AT45D_CFG_PWR_OF_TWO_PAGE_SIZE==0 ? 8 : 0))
#elif (AT45D_CFG_DEVICE == AT45DB161)
#define AT45D_PAGES             4096ul
#define AT45D_PAGE_SIZE         (512 + (AT45D_CFG_PWR_OF_TWO_PAGE_SIZE==0 ? 16 : 0))
#elif (AT45D_CFG_DEVICE == AT45DB321)
#define AT45D_PAGES             8192ul
#define AT45D_PAGE_SIZE         (512 + (AT45D_CFG_PWR_OF_TWO_PAGE_SIZE==0 ? 16 : 0))
#elif (AT45D_CFG_DEVICE == AT45DB642)
#define AT45D_PAGES             8192ul
#define AT45D_PAGE_SIZE         (1024 + (AT45D_CFG_PWR_OF_TWO_PAGE_SIZE==0 ? 32 : 0))
#else
#error "Invalid AT45D device specified"
#endif

/// Flash size (in bytes)
#define AT45D_FLASH_SIZE_BYTES   (AT45D_PAGES*AT45D_PAGE_SIZE)

/// Maximum adress
#define AT45D_ADR_MAX            (AT45D_FLASH_SIZE_BYTES-1)

/// @name Status register
//@{
#define AT45D_STATUS_READY          7       ///< Ready flag
#define AT45D_STATUS_COMP           6       ///< Compare flag
#define AT45D_STATUS_DENSITY_MASK   0x3C    ///< Density mask
#define AT45D_STATUS_PROTECT        1       ///< Protect flag
#define AT45D_STATUS_PAGE_SIZE      0       ///< Page size flag
//@}

/// Maximum SPI Clock rate
//#define AT45D_MAX_SPI_CLOCK_HZ  33000000
#define AT45D_MAX_SPI_CLOCK_HZ  20000000

/// SPI Clock / Data phase
#define AT45D_SPI_MODE          SPI_MODE0
/// SPI Data order
#define AT45D_SPI_DATA_ORDER    SPI_DATA_ORDER_MSB

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// Address size can be optimized, depending on maximum size of FLASH
#if (AT45D_ADR_MAX <= 0xffff)
typedef u16_t at45d_adr_t;
#else
typedef u32_t at45d_adr_t;
#endif

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
    Initialise driver

    @param handle    SPI handle to use for SPI slave device
 */
void at45d_init(spi_handle_t handle);

/// Power down device to minimise power consumption
void at45d_power_down(void);

/// Power up device to resume communication
void at45d_resume_from_power_down(void);

/**
    Read data from DataFlash.
    
    This function reads data from DataFlash and stores it in the specified
    buffer.

    @param[out] buffer          Buffer to store read data
    @param[in]  address         0 to AT45D_ADR_MAX
    @param[in]  nr_of_bytes     Number of bytes to read
    
    @return u16_t               Number of bytes actually read
 */
u16_t at45d_rd(void        *buffer,
               at45d_adr_t address,
               u16_t       nr_of_bytes);

/**
    Read a page from DataFlash.
    
    This function reads a page of data from DataFlash and stores it in the 
    specified buffer. The buffer must be at least AT45D_PAGE_SIZE bytes in size
    to accomodate a full page.

    The DataFlash has AT45D_PAGES pages.

    @param[out] buffer          Buffer to store read data
    @param[in]  page            0 to (AT45D_PAGES-1)
    
 */
void at45d_rd_page(void  *buffer,
                   u16_t  page);

/**
    Partial read of data in a page of DataFlash.

    This function reads part of a page of data from DataFlash and stores it in
    the specified buffer. The buffer must be at least @b nr_of_bytes in size
    to the read data.

    @note
    
    Only read up to the end of the specified page. If the page boundary is
    exceeded, the index will wrap to the start of the page, i.e. only the
    content of the specified page will be read.

    @param[out] buffer              Buffer to store read data
    @param[in]  page                0 to (AT45D_PAGES-1)
    @param[in]  start_byte_in_page  Index of first byte to read (0 to
                                    AT45D_PAGE_SIZE - 1)
    @param[in]  nr_of_bytes         Number of bytes to read

 */
void at45d_rd_page_offset(void  *buffer,
                          u16_t page,
                          u16_t start_byte_in_page,
                          u16_t nr_of_bytes);

/**
    Write a page from DataFlash.
    
    This function writes a page of data to DataFlash using the specified 
    buffer as the source. The buffer must contain at least AT45D_PAGE_SIZE bytes
    of data.

    The DataFlash has AT45D_PAGES pages.

    @param[in] buffer  Buffer containing data to be written
    @param[in]  page   0 to (AT45D_PAGES-1)
    
 */
void at45d_wr_page(const void *buffer,
                   u16_t       page);

/**
    Partial write of data in a page of DataFlash.

    This function writes a part of a page of data to DataFlash using the 
    specified buffer as source. The buffer must contain at least @b nr_of_bytes
    of data.

    @note
    
    Only write up to the end of the specified page. If the page boundary is
    exceeded, the index will wrap to the start of the page, i.e. only the
    content of the specified page will be written.

    @param[in]  buffer              Buffer containing data to be written
    @param[in]  page                0 to (AT45D_PAGES-1)
    @param[in]  start_byte_in_page  Index of first byte to write (0 to
                                    AT45D_PAGE_SIZE - 1)
    @param[in]  nr_of_bytes         Number of bytes to write

 */
void at45d_wr_page_offset(const void *buffer, 
                          u16_t      page,   
                          u16_t      start_byte_in_page,                                    
                          u16_t      nr_of_bytes);

/**
    Erase a page of DataFlash.
    
    This function erases a page of DataFlash. The DataFlash has AT45D_PAGES
    pages.

    @param[in]  page   0 to (AT45D_PAGES-1)
 */
void at45d_erase_page(u16_t page);

/**
    Check if DataFlash is ready for the next read or write access.

    When data is written to DataFlash, the microcontroller must wait until the
    operation is finished, before the next one is attempted. This function
    allows the microcontroller to do something else while waiting for the
    operation to finish.

   @retval TRUE     DataFlash is ready for next read or write access.
   @retval FALSE    DataFlash is busy writing.
 */
bool_t at45d_ready(void);

/**
    Read the status register of the DataFlash.

    The status register contains flags (e.g. AT45D_STATUS_READY) and the density
    value (e.g. AT45D_DENSITY). This function can be used to check that a valid
    and working DataFlash device is connected.

    @return u8_t Status register value
 */
u8_t at45d_get_status(void);

/**
    Check if page size of the DataFlash is a power of two.

    The status register contains a flag AT45D_STATUS_PAGE_SIZE that reports the
    page size: 1 = power of two, 0 = not a power of two.

    @return bool_t TRUE  Page size is a power of two
                   FALSE Page size is not a power of two
 */
bool_t at45d_page_size_is_pwr_of_two(void);

/**
    Sets the page size of the DataFlash to a power of two.

    A one-time programmable configuration register is written to configure the
    device for "power of 2" page size. Once it has been written, it can not be
    reconfigured again.
    
    The device must be power cycled for this configuration bit to come into
    effect.
    
    @return bool_t TRUE     page size was set to a power of two
                   FALSE    page size was already set to a power of two
 */
bool_t at45d_set_page_size_to_pwr_of_two(void);

/* _____MACROS_______________________________________________________________ */

/// @}
#ifdef __cplusplus
}
#endif

#endif // #ifndef __AT45D_H__
