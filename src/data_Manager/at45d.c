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

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include <data_Manager/at45d.h>
#include <BSP_Waveshare/bsp_waveshare.h>

/* _____LOCAL DEFINITIONS____________________________________________________ */
/// @name Read commands
//@{
#define AT45D_CMD_CONTINUOUS_ARRAY_READ             0xe8
#define AT45D_CMD_MAIN_MEMORY_PAGE_READ             0xd2
#define AT45D_CMD_STATUS_REGISTER_READ              0xd7
//@}

/// @name Program and Erase commands
//@{
#define AT45D_CMD_BUFFER1_WRITE                     0x84
#define AT45D_CMD_BUFFER2_WRITE                     0x87

#define AT45D_CMD_BUF1_TO_MAIN_PAGE_PRG_W_ERASE     0x83
#define AT45D_CMD_BUF2_TO_MAIN_PAGE_PRG_W_ERASE     0x86

#define AT45D_CMD_BUF1_TO_MAIN_PAGE_PRG_WO_ERASE    0x88
#define AT45D_CMD_BUF2_TO_MAIN_PAGE_PRG_WO_ERASE    0x89

#define AT45D_CMD_MAIN_MEM_PROG_THROUGH_BUF1        0x82
#define AT45D_CMD_MAIN_MEM_PROG_THROUGH_BUF2        0x85

#define AT45D_CMD_PAGE_ERASE                        0x81
//@}

/// @name Additional commands
//@{
#define AT45D_CMD_MAIN_MEM_PAGE_TO_BUF1             0x53
#define AT45D_CMD_MAIN_MEM_PAGE_TO_BUF2             0x55
//@}

#define AT45D_CMD_MAIN_POWER_DOWN                   0xb9
#define AT45D_CMD_MAIN_RESUME_FROM_POWER_DOWN       0xab

/* _____MACROS_______________________________________________________________ */
#define ATD45D_SSPx		LPC_SSP1
#define spi_cs_hi() 	Chip_GPIO_SetPinOutHigh(LPC_GPIO, 1, 18 )
#define spi_cs_lo()  	Chip_GPIO_SetPinOutLow(LPC_GPIO, 1, 18 )

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
static bool_t       at45d_ready_flag;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
//#if ((AT45D_PAGE_SIZE != 256) && (AT45D_PAGE_SIZE != 264))
//#error "This driver does not work for any other page size yet. See comment!"
/*
   Unfortunately this driver still needs work to cater for other page sizes. 
    
   When the address is sent, the specified page value is shifted by a fixed amount, 
   but actually the amount of shift depends on the page size (?) 
 */
//#endif

static char spi_RW_u8(char data) {
	while (!(ATD45D_SSPx->SR & SSP_STAT_TFE));
	ATD45D_SSPx->DR = data;	// send data
	while (!(ATD45D_SSPx->SR & SSP_STAT_RNE));
	return ATD45D_SSPx->DR;	// Receive data
}

static void spi_wr_data(const void* buf, size_t nr_of_bytes)
{
	const u8_t *data_u8 = (u8_t *)buf;
	while (nr_of_bytes--)
	{
		spi_RW_u8(*data_u8++);
	}
}

static void spi_rd_data(char* buffer, size_t nr_of_bytes){
	while (nr_of_bytes--){
		*buffer++ = spi_RW_u8(0);
	}
}


static void at45d_tx_adr(u16_t page, u16_t start_byte_in_page)
{
#if VAL_IS_PWR_OF_TWO(AT45D_PAGE_SIZE)
	spi_RW_u8((u8_t)(page>>8)&0xFF);
	spi_RW_u8((u8_t)(page&0xFF));
	spi_RW_u8((u8_t)(start_byte_in_page&0xFF));
#else
    spi_RW_u8((page>>7)&0xFF);
    spi_RW_u8(((page<<1)|(start_byte_in_page>>8))&0xFF);
    spi_RW_u8(start_byte_in_page&0xFF);
#endif
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void at45d_init(spi_handle_t handle)
{
//	Chip_Clock_SetPCLKDiv(SYSCTL_CLOCK_SSP1, SYSCTL_CLKDIV_1);
//	Board_SSP_Init(LPC_SSP1);
//	Chip_SSP_Init(LPC_SSP1);
//	Chip_SSP_SetMaster(LPC_SSP1, TRUE);
//	Chip_SSP_SetBitRate(LPC_SSP1, AT45D_MAX_SPI_CLOCK_HZ);
//	Chip_SSP_Enable(LPC_SSP1);

//	LPC_SYSCTL->PCLKSEL[0] |= (SYSCTL_CLKDIV_1 << 20);
//	LPC_SYSCTL->PCONP |= _BIT(SYSCTL_CLOCK_SSP1);
//	LPC_SSP1->CPSR |= 2; /* CPSDVSR=2 */
//	LPC_SSP1->CR0 |= 0x0007; /* Set mode: SPI mode 0, 8-bit */
//	LPC_SSP1->CR1 |= 0x2; /* Enable SSP with Master */
//	LPC_SSP1->CR0 |= ((SystemCoreClock/(2*AT45D_MAX_SPI_CLOCK_HZ))-1)<<8;

	/*
	 * Must be initialized in Main function calling Board_SSP_INit
	 */
	Chip_GPIO_SetPinDIROutput(LPC_GPIO1, 1, 18);
}

void at45d_power_down(void)
{

    // Select DataFlash
    spi_cs_lo();

    // Send command
    spi_RW_u8(AT45D_CMD_MAIN_POWER_DOWN);

    // Deselect DataFlash
    spi_cs_hi();
}

void at45d_resume_from_power_down(void)
{
    // Select DataFlash
    spi_cs_lo();

    // Send command
    spi_RW_u8(AT45D_CMD_MAIN_RESUME_FROM_POWER_DOWN);

    // Deselect DataFlash
    spi_cs_hi();
}

u16_t at45d_rd(void       *buffer,
               at45d_adr_t address,
               u16_t       nr_of_bytes)
{
    at45d_adr_t max_bytes_to_read;
    u16_t       page;
    u16_t       start_byte_in_page;

    // See if specified address is out of bounds
    if(address > AT45D_ADR_MAX)
    {
        return 0;
    }

    // See if "number of bytes to read" should be clipped
    max_bytes_to_read = AT45D_ADR_MAX - address + 1;
    if(nr_of_bytes > max_bytes_to_read)
    {
        nr_of_bytes = max_bytes_to_read;
    }

    // Wait until DataFlash is not busy
    while(!at45d_ready())
    {
        ;
    }

    // Select DataFlash
    spi_cs_lo();

    // Send command
    spi_RW_u8(AT45D_CMD_CONTINUOUS_ARRAY_READ);

    // Calculate page, offset and number of bytes remaining in page
#if VAL_IS_PWR_OF_TWO(AT45D_PAGE_SIZE)
    page               = address >> 8;
    start_byte_in_page = address & 0xff;
#else
    page               = address / AT45D_PAGE_SIZE;
    start_byte_in_page = address % AT45D_PAGE_SIZE;
#endif
    
    // Send address
    at45d_tx_adr(page, start_byte_in_page);

    // Send dont-care bits
    spi_RW_u8(0x00);
    spi_RW_u8(0x00);
    spi_RW_u8(0x00);
    spi_RW_u8(0x00);

    // Read data
    spi_rd_data(buffer, nr_of_bytes);

    // Deselect DataFlash
    spi_cs_hi();

    return nr_of_bytes;
}

void at45d_rd_page(void* buffer, u16_t page)
{
    // Wait until DataFlash is not busy
    while(!at45d_ready())
    {
        ;
    }

    // Select DataFlash
    spi_cs_lo();

    // Send command
    spi_RW_u8(AT45D_CMD_MAIN_MEMORY_PAGE_READ);

    // Send address
    at45d_tx_adr(page, 0);

    // Send dont-care bits
    spi_RW_u8(0x00);
    spi_RW_u8(0x00);
    spi_RW_u8(0x00);
    spi_RW_u8(0x00);

    // Read data
    spi_rd_data(buffer, AT45D_PAGE_SIZE);

    // Deselect DataFlash
    spi_cs_hi();
}

void at45d_rd_page_offset(void *buffer,
                          u16_t page,
                          u16_t start_byte_in_page,
                          u16_t nr_of_bytes)
{
    // Wait until DataFlash is not busy
    while(!at45d_ready())
    {
        ;
    }

    // Select DataFlash
    spi_cs_lo();

    // Send command
    spi_RW_u8(AT45D_CMD_CONTINUOUS_ARRAY_READ);

    // Send address
    at45d_tx_adr(page, start_byte_in_page);

    // Send dont-care bits
    spi_RW_u8(0x00);
    spi_RW_u8(0x00);
    spi_RW_u8(0x00);
    spi_RW_u8(0x00);

    // Read data
    spi_rd_data(buffer, nr_of_bytes);    

    // Deselect DataFlash
    spi_cs_hi();
}

void at45d_wr_page(const void* buffer, u16_t page)
{
    // Wait until DataFlash is not busy
    while(!at45d_ready())
    {
        ;
    }

    // Select DataFlash
    spi_cs_lo();

    // Send command
    spi_RW_u8((u8_t)AT45D_CMD_MAIN_MEM_PROG_THROUGH_BUF1);

    // Send address
    at45d_tx_adr(page, 0);

    // Send data to be written
    spi_wr_data(buffer, (u8_t)AT45D_PAGE_SIZE);

    // Deselect DataFlash
    spi_cs_hi();

    // Set flag to busy
    at45d_ready_flag = FALSE;
}

void at45d_wr_page_offset(const void* buffer,
                          u16_t       page,
                          u16_t       start_byte_in_page,
                          u16_t       nr_of_bytes)
{
    u16_t i;
    u8_t *bufffer_u8 = (u8_t *)buffer;

    // Wait until DataFlash is not busy
    while(!at45d_ready())
    {
        ;
    }   

    // Select DataFlash
    spi_cs_lo();

    // Send command
    spi_RW_u8(AT45D_CMD_BUFFER1_WRITE);

    // Send start byte in buffer
    spi_RW_u8(0x00);
    spi_RW_u8(0x00);
    spi_RW_u8(0x00);

    // Fill buffer with data to be written (other bytes are 0xFF to leave them unchanged)
    for(i=0; i<AT45D_PAGE_SIZE; i++)
    {
        if(  (i >= start_byte_in_page                  )
           &&(i  < start_byte_in_page + nr_of_bytes)  )
        {
        	spi_RW_u8(*bufffer_u8++);
        }
        else
        {
            // Leave unchanged
        	spi_RW_u8(0xff);
        }
    }

    // Deselect DataFlash
    spi_cs_hi();

    // Set flag to busy
    at45d_ready_flag = FALSE;

    // Wait until DataFlash is not busy
    while(!at45d_ready())
    {
        ;
    }

    // Select DataFlash
    spi_cs_lo();

    // Send command
    spi_RW_u8(AT45D_CMD_BUF1_TO_MAIN_PAGE_PRG_W_ERASE);

    // Send address
    at45d_tx_adr(page, 0);    

    // Deselect DataFlash
    spi_cs_hi();

    // Set flag to busy
    at45d_ready_flag = FALSE;
}

void at45d_erase_page(u16_t page)
{
    // Wait until DataFlash is not busy
    while(!at45d_ready())
    {
        ;
    }
    // Select DataFlash
    spi_cs_lo();

    // Send command
    spi_RW_u8(AT45D_CMD_PAGE_ERASE);

    // Send address
    at45d_tx_adr(page, 0);

    // Deselect DataFlash
    spi_cs_hi();

    // Set flag to busy
    at45d_ready_flag = FALSE;
}

bool_t at45d_ready(void)
{
    u8_t data;

    // If flag has already been set, then take short cut
    if(at45d_ready_flag)
    {
        return TRUE;
    }

    // Get DataFlash status
    data = at45d_get_status();

    // See if DataFlash is ready
    if(BIT_IS_HI(data, AT45D_STATUS_READY))
    {
        // Set flag
        at45d_ready_flag = TRUE;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

u8_t at45d_get_status(void)
{
    u8_t data;
    // Select DataFlash
    spi_cs_lo();

    // Send command
    spi_RW_u8(AT45D_CMD_STATUS_REGISTER_READ);

    // Read status
    data = spi_RW_u8(0);

    // Deselect DataFlash
    spi_cs_hi();

    return data;
}

bool_t at45d_page_size_is_pwr_of_two(void)
{
    u8_t data = at45d_get_status();

    if(BIT_IS_HI(data, AT45D_STATUS_PAGE_SIZE))
    {
        // Page size is a power of two
        return TRUE;
    }
    else
    {
        // Page size is not a power of two
        return FALSE;
    }
}

bool_t at45d_set_page_size_to_pwr_of_two(void)
{
    if(at45d_page_size_is_pwr_of_two())
    {
        // Page size is already a power of two
        return FALSE;
    }

    // Wait until DataFlash is not busy
    while(!at45d_ready())
    {
        ;
    }

    // Select DataFlash
    spi_cs_lo();

    // Send command
    spi_RW_u8(0x3d);
    spi_RW_u8(0x2a);
    spi_RW_u8(0x80);
    spi_RW_u8(0xa6);

    // Deselect DataFlash
    spi_cs_hi();

    // Wait until DataFlash is not busy
    while(!at45d_ready())
    {
        ;
    }

    return TRUE;
}
