/*
 * sdcard.h
 *
 *  Created on: Dec 3, 2019
 *      Author: dsolano
 */

#ifndef INC_SDCARD_H_
#define INC_SDCARD_H_

#include <lpc_types.h>
#include <stdint.h>
#include <chip.h>

/* SPI clock rate setting.
SSP0_CLK = SystemCoreClock / divider,
The divider must be a even value between 2 and 254!
In SPI mode, max clock speed is 20MHz for MMC and 25MHz for SD */
#define SPI_CLOCKRATE_LOW   400000
#define SPI_CLOCKRATE_HIGH  25000000


/* type defintion */
typedef unsigned char    SD_BOOL;
#define SD_TRUE     1
#define SD_FALSE    0

#ifndef NULL
 #ifdef __cplusplus              // EC++
  #define NULL          0
 #else
  #define NULL          ((void *) 0)
 #endif
#endif

/* Memory card type definitions */
#define CARDTYPE_UNKNOWN        0
#define CARDTYPE_MMC            1   /* MMC */
#define CARDTYPE_SDV1           2   /* V1.x Standard Capacity SD card */
#define CARDTYPE_SDV2_SC        3   /* V2.0 or later Standard Capacity SD card */
#define CARDTYPE_SDV2_HC        4   /* V2.0 or later High/eXtended Capacity SD card */

/* SD/MMC card configuration */
typedef struct tagCARDCONFIG
{
    uint32_t sectorsize;    /* size (in byte) of each sector, fixed to 512bytes */
    uint32_t sectorcnt;     /* total sector number */
    uint32_t blocksize;     /* erase block size in unit of sector */
	uint8_t  ocr[4];		/* OCR */
	uint8_t  cid[16];		/* CID */
	uint8_t  csd[16];		/* CSD */
} CARDCONFIG;

/* Public variables */
extern uint8_t CardType;
extern CARDCONFIG CardConfig;


/* Public functions */
SD_BOOL     SD_Init (void);
SD_BOOL     SD_ReadSector (uint32_t sect, uint8_t *buf, uint32_t cnt);
SD_BOOL     SD_WriteSector (uint32_t sect, const uint8_t *buf, uint32_t cnt);
SD_BOOL     SD_ReadConfiguration (void);
void        disk_timerproc (void);

#endif /* INC_SDCARD_H_ */
