/**********************************************************************
* $Id$		sdram_HY57V281620_X2.c			2011-06-02
*//**
* @file		sdram_HY57V281620_X2.c
* @brief	Contains all functions support for SAMSUNG K4S561632J
*			(supported on LPC1788 IAR Olimex Start Kit Rev.B)
* @version	1.0
* @date		02. June. 2011
* @author	NXP MCU SW Application Team
* 
* Copyright(C) 2011, NXP Semiconductor
* All rights reserved.
*
***********************************************************************
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
* NXP Semiconductors assumes no responsibility or liability for the
* use of the software, conveys no license or title under any patent,
* copyright, or mask work right to the product. NXP Semiconductors
* reserves the right to make changes in the software without
* notification. NXP Semiconductors also make no representation or
* warranty that such application will be suitable for the specified
* use without further testing or modification.
**********************************************************************/
#include <chip.h>
#include "gfx/sdram_HY57V281620_X2.h"

/* EMC clock delay */
#define CLK0_DELAY 7

STATIC const IP_EMC_DYN_CONFIG_T IS42S32800D_config = {
	EMC_NANOSECOND(64000000 / 4096),
	0x01,				/* Command Delayed */
	3,					/* tRP */
	7,					/* tRAS */
	EMC_NANOSECOND(70),	/* tSREX */
	EMC_CLOCK(0x01),	/* tAPR */
	EMC_CLOCK(0x05),	/* tDAL */
	EMC_NANOSECOND(12),	/* tWR */
	EMC_NANOSECOND(60),	/* tRC */
	EMC_NANOSECOND(60),	/* tRFC */
	EMC_NANOSECOND(70),	/* tXSR */
	EMC_NANOSECOND(12),	/* tRRD */
	EMC_CLOCK(0x02),	/* tMRD */
	{
		{
			EMC_ADDRESS_DYCS0,	/* EA Board uses DYCS0 for SDRAM */
			2,	/* RAS */

			EMC_DYN_MODE_WBMODE_PROGRAMMED |
			EMC_DYN_MODE_OPMODE_STANDARD |
			EMC_DYN_MODE_CAS_2 |
			EMC_DYN_MODE_BURST_TYPE_SEQUENTIAL |
			EMC_DYN_MODE_BURST_LEN_4,

			EMC_DYN_CONFIG_DATA_BUS_32 |
			EMC_DYN_CONFIG_LPSDRAM |
			EMC_DYN_CONFIG_8Mx16_4BANKS_12ROWS_9COLS |
			EMC_DYN_CONFIG_MD_SDRAM
		},
		{0, 0, 0, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0}
	}
};

/* NorFlash timing and chip Config */
STATIC const IP_EMC_STATIC_CONFIG_T SST39VF320_config = {
	0,
	EMC_STATIC_CONFIG_MEM_WIDTH_16 |
	EMC_STATIC_CONFIG_CS_POL_ACTIVE_LOW |
	EMC_STATIC_CONFIG_BLS_HIGH /* |
							      EMC_CONFIG_BUFFER_ENABLE*/,

	EMC_NANOSECOND(0),
	EMC_NANOSECOND(35),
	EMC_NANOSECOND(70),
	EMC_NANOSECOND(70),
	EMC_NANOSECOND(40),
	EMC_CLOCK(4)
};

/* NandFlash timing and chip Config */
STATIC const IP_EMC_STATIC_CONFIG_T K9F1G_config = {
	1,
	EMC_STATIC_CONFIG_MEM_WIDTH_8 |
	EMC_STATIC_CONFIG_CS_POL_ACTIVE_LOW |
	EMC_STATIC_CONFIG_BLS_HIGH /* |
							      EMC_CONFIG_BUFFER_ENABLE*/,

	EMC_NANOSECOND(0),
	EMC_NANOSECOND(35),
	EMC_NANOSECOND(70),
	EMC_NANOSECOND(70),
	EMC_NANOSECOND(40),
	EMC_CLOCK(4)
};

/* Pin muxing configuration EMC for SDRAM*/
STATIC const PINMUX_GRP_T EMCpinmuxing[] = {
		{2,16,0x21},		/* init EMC_CAS */
		{2,17,0x21},		/* init EMC_RAS */
		{2,18,0x21},		/* init EMC_CLK0 */
		{2,20,0x21},		/* init EMC_DYCS0 */
		{2,24,0x21},		/* init EMC_CKE0 */
		{2,28,0x21},		/* init EMC_DQM0 */
		{2,29,0x21},		/* init EMC_DQM1 */
		{2,30,0x21},		/* init EMC_DQM2 */
		{2,31,0x21},			/* init EMC_DQM3 */

		/************** init SDRAM DATA PIN**********/

		{3,0,0x21},			/* init EMC_D0 */
		{3,1,0x21},			/* init EMC_D1 */
		{3,2,0x21},			/* init EMC_D2 */
		{3,3,0x21},			/* init EMC_D3 */
		{3,4,0x21},			/* init EMC_D4 */
		{3,5,0x21},			/* init EMC_D5 */
		{3,6,0x21},			/* init EMC_D6 */
		{3,7,0x21},			/* init EMC_D7 */
		{3,8,0x21},			/* init EMC_D8 */
		{3,9,0x21},			/* init EMC_D9 */
		{3,10,0x21},		/* init EMC_D10 */
		{3,11,0x21},		/* init EMC_D11 */
		{3,12,0x21},		/* init EMC_D12 */
		{3,13,0x21},		/* init EMC_D13 */
		{3,14,0x21},		/* init EMC_D14 */
		{3,15,0x21},		/* init EMC_D15 */
		{3,16,0x21},		/* init EMC_D16 */
		{3,17,0x21},		/* init EMC_D17 */
		{3,18,0x21},		/* init EMC_D18 */
		{3,19,0x21},		/* init EMC_D19 */
		{3,20,0x21},		/* init EMC_D20 */
		{3,21,0x21},		/* init EMC_D21 */
		{3,22,0x21},		/* init EMC_D22 */
		{3,23,0x21},		/* init EMC_D23 */
		{3,24,0x21},		/* init EMC_D24 */
		{3,25,0x21},		/* init EMC_D25 */
		{3,26,0x21},		/* init EMC_D26 */
		{3,27,0x21},		/* init EMC_D27 */
		{3,28,0x21},		/* init EMC_D28 */
		{3,29,0x21},		/* init EMC_D29 */
		{3,30,0x21},		/* init EMC_D30 */
		{3,31,0x21},		/* init EMC_D31 */

		/************** init SDRAM ADDR PIN**********/
		{4,0,0x21},			/* init EMC_A0 */
		{4,1,0x21},			/* init EMC_A1 */
		{4,2,0x21},			/* init EMC_A2 */
		{4,3,0x21},			/* init EMC_A3 */
		{4,4,0x21},			/* init EMC_A4 */
		{4,5,0x21},			/* init EMC_A5 */
		{4,6,0x21},			/* init EMC_A6 */
		{4,7,0x21},			/* init EMC_A7 */
		{4,8,0x21},			/* init EMC_A8 */
		{4,9,0x21},			/* init EMC_A9 */
		{4,10,0x21},		/* init EMC_A10 */
		{4,11,0x21},		/* init EMC_A11 */
		{4,12,0x21},		/* init EMC_A12 */
		{4,13,0x21},		/* init EMC_A13 */
		{4,14,0x21},		/* init EMC_A14 */

		  /************** init SDRAM WE PIN**********/
		{4,25,0x21}			/* init EMC_WE */

};

/*********************************************************************//**
 * @brief 		Initialize external SDRAM memory Micron MT48LC8M32LFB5
 *				256Mbit(8M x 32)
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void SDRAMInit( void )
{
	volatile uint32_t i;
	//volatile unsigned long Dummy;

/*
************************************
  init SDRAM  Config PIN
************************************
*/
/************** init SDRAM **********/

	/* Setup LCD level pin muxing */
	Chip_IOCON_SetPinMuxing(LPC_IOCON, EMCpinmuxing, sizeof(EMCpinmuxing) / sizeof(PINMUX_GRP_T));

/*
************************************
  init SDRAM CONTROLLER
************************************
*/

	/* Setup EMC Delays */
	/* Move all clock delays together */
	LPC_SYSCTL->EMCDLYCTL = (CLK0_DELAY) | (CLK0_DELAY << 8) | (CLK0_DELAY << 16 | (CLK0_DELAY << 24));

	/* Setup EMC Clock Divider for divide by 2 */
	/* Setup EMC clock for a divider of 2 from CPU clock. Enable EMC clock for
	   external memory setup of DRAM. */
	Chip_Clock_SetEMCClockDiv(SYSCTL_EMC_DIV2);
	Chip_SYSCTL_PeriphReset(SYSCTL_RESET_EMC);

	/* Init EMC Controller -Enable-LE mode- clock ratio 1:1 */
	Chip_EMC_Init(1, 0, 0);

	/* Init EMC Dynamic Controller */
	Chip_EMC_Dynamic_Init((IP_EMC_DYN_CONFIG_T *) &IS42S32800D_config);

	/* Init EMC Static Controller CS0 */
	Chip_EMC_Static_Init((IP_EMC_STATIC_CONFIG_T *) &SST39VF320_config);


	/* Init EMC Static Controller CS1 */
	Chip_EMC_Static_Init((IP_EMC_STATIC_CONFIG_T *) &K9F1G_config);

	/* EMC Shift Control */
	LPC_SYSCTL->SCS |= 1;


  /* Enable the EMC POWER */
	LPC_SYSCTL->PCONP   	|= 0x00000800;

	/* 
  Delay Control register (EMCDLYCTL - 0x400F C1DC)
  The EMCDLYCTL register controls on-chip programmable delays that can b used to fine 
  tune timing to external SDRAM memories. Dela ys can be configured in increments of 
  approximately 250 picoseconds up to a maximum of roughly 7.75 ns.
	*/
	
	/*
	Programmable delay value for EMC outputs in command delayed mode
	The delay amount is roughly (CMDDLY+1) * 250 picoseconds
	*/
	LPC_SYSCTL->EMCDLYCTL |= (8<<0);

	/*
	Programmable delay value for the feedback clock that controls input data sampling
	The delay amount is roughly (FBCLKDLY+1) * 250 picoseconds
	*/
	LPC_SYSCTL->EMCDLYCTL |=(8<<8);

	/*
  Programmable delay value for the CLKOUT0 output. This would typically be used in clock 
  delayed mode. The delay amount is roughly (CLKOUT0DLY+1) * 250 picoseconds.
	*/
	LPC_SYSCTL->EMCDLYCTL |= (0x08 <<16);


  /* 
	Confige the EMC Register 
	*/
	/*
	EMC Control register (EMCControl - 0x2009 C000)
	EMC Enable (E) = 1;
	Address mirror (M) = 0;	Normal memory map.
	Low-power mode (L) = 0; Normal mode (warm reset value).
	*/
	LPC_EMC->CONTROL =1;

	/*
	Dynamic Memory Read Configuration register (EMCDynamicReadConfig - 0x2009 C028)
  1:0 Read data  strategy (RD)
  00 Clock out delayed strategy, using CLKOUT (command not delayed, clock out 
     delayed). POR reset value.
  01 Command delayed strategy, using EMCCLKDELAY (command delayed, clock out 
     not delayed).
  10 Command delayed strategy plus one clock cycle, using EMCCLKDELAY 
     (command delayed, clock out not delayed).
  11 Command delayed strategy plus  two clock cycles, using EMCCLKDELAY 
     (command delayed, clock out not delayed)
	*/
	LPC_EMC->DYNAMICREADCONFIG = 1;

  /* 
	Dynamic Memory RAS & CAS Delay registers
	The EMCDynamicRasCas0-3 registers enable you to program the RAS and CAS 
  latencies for the relevant dynamic memory
	1:0 RAS latency (active to read/write delay) (RAS)
       00 Reserved. 11
       01 One CCLK cycle.
       10 Two CCLK cycles.
       11 Three CCLK cycles (POR reset value).
  9:8 CAS latency (CAS) 
	    00 Reserved. 11
      01 One CCLK cycle.
      10 Two CCLK cycles.
      11 Three CCLK cycles (POR reset value).
	*/
	LPC_EMC->DYNAMICRASCAS0 = 0;
	LPC_EMC->DYNAMICRASCAS0 |=(3<<8);
	LPC_EMC->DYNAMICRASCAS0 |= (3<<0);

	/*
	Dynamic Memory Precharge Command Period registe (EMCDynamictRP - 0x2009 C030)
	The EMCDynamicTRP register enables you to program the precharge command period, 
  tRP.
	3:0 Precharge command period (tRP)
      0x0 - 0xE n + 1 clock cycles. The delay is in EMCCLK cycles.
      0xF 16 clock cycles (POR reset value).
	*/
	LPC_EMC->DYNAMICRP = P2C(SDRAM_TRP);
	/*
	Dynamic Memory Active to Precharge Command Period register(EMCDynamictRAS - 0x2009 C034)
  The EMCDynamicTRAS register enables you to program the active to precharge command period, tRAS.
	3:0 Precharge command period (tRAS)
      0x0 - 0xE n + 1 clock cycles. The delay is in EMCCLK cycles.
      0xF 16 clock cycles (POR reset value).  
	*/
	LPC_EMC->DYNAMICRAS = P2C(SDRAM_TRAS);
	/*
	Dynamic Memory Se lf-refresh Exit Time register(EMCDynamictSREX - 0x2009 C038)
  The EMCDynamicTSREX register enables you  to program the self-refresh exit time
  3:0 Self-refresh exit time (tSREX)
      0x0 - 0xE n + 1 clock cycles. The delay is in CCLK cycles.
      0xF 16 clock cycles (POR reset value).
	*/
	LPC_EMC->DYNAMICSREX = P2C(SDRAM_TXSR);
	/*
	Dynamic Memory Last Data Out to Active Time register (EMCDynamictAPR - 0x2009 C03C)
  The EMCDynamicTAPR register enables you to program the last-data-out to active 
  command time, tAPR.
  3:0 Last-data-out to active command time (tAPR)
      0x0 - 0xE n + 1 clock cycles. The delay is in CCLK cycles.
      0xF 16 clock cycles (POR reset value).
	*/
	LPC_EMC->DYNAMICAPR = SDRAM_TAPR;
	/*
	Dynamic Memory Data-in to  Active Command Time register (EMCDynamictDAL - 0x2009 C040)
  The EMCDynamicTDAL register enables you to program the data-in to active command time, tDAL
  3:0 Data-in to active command (tDAL)
      0x0 - 0xE n + 1 clock cycles. The delay is in CCLK cycles.
      0xF 16 clock cycles (POR reset value).
	*/
	LPC_EMC->DYNAMICDAL = SDRAM_TDAL+P2C(SDRAM_TRP);
	/*
	Dynamic Memory Wr ite Recovery Time regist er (EMCDynamictWR - 0x2009 C044)
  The EMCDynamicTWR register enables you to pr ogram the write recovery time, tWR.
  3:0 Write recovery time (tWR)
      0x0 - 0xE n + 1 clock cycles. The delay is in CCLK cycles.
      0xF 16 clock cycles (POR reset value).
	*/
	LPC_EMC->DYNAMICWR = SDRAM_TWR;
	/*
	Dynamic Memory Active to  Active Command Period register (EMCDynamictRC - 0x2009 C048)
  The EMCDynamicTRC register enables you to program the active to active command period, tRC.
  3:0 Active to active command period (tRC)
      0x0 - 0x1E n + 1 clock cycles. The delay is in CCLK cycles.
      0xF 32 clock cycles (POR reset value).
	*/
	LPC_EMC->DYNAMICRC = P2C(SDRAM_TRC);
	/*
	Dynamic Memory Auto-refresh Period register (EMCDynamictRFC - 0x2009 C04C)
  The EMCDynamicTRFC register enables you to program the auto-refresh period, 
	and auto-refresh to active command period, tRFC.
  4:0 Auto-refresh period and auto-refresh to active command period (tRFC)
      0x0 - 0x1E n + 1 clock cycles. The delay is in CCLK cycles.
      0xF 32 clock cycles (POR reset value).
	*/
	LPC_EMC->DYNAMICRFC = P2C(SDRAM_TRFC);
	/*
	Dynamic Memory Exit Self-ref resh register (EMCDynamictXSR - 0x2009 C050)
	The EMCDynamicTXSR register enables you to program the exit self-refresh to active 
	command time, tXSR. 
  4:0 Exit self-refresh to active command time (tXSR)
      0x0 - 0x1E n + 1 clock cycles. The delay is in CCLK cycles.
      0xF 32 clock cycles (POR reset value).
	*/
	LPC_EMC->DYNAMICXSR = P2C(SDRAM_TXSR);
	/*
	Dynamic Memory Active Bank A to Active Ba nk B Time register(EMCDynamictR RD - 0x2009 C054)
	The EMCDynamicTRRD register enables you to program the active bank A to active bank B latency, tRRD.
  3:0 Active bank A to active bank B latency (tRRD)
      0x0 - 0x1E n + 1 clock cycles. The delay is in CCLK cycles.
      0xF        16 clock cycles (POR reset value).
	*/
	LPC_EMC->DYNAMICRRD = P2C(SDRAM_TRRD);
	/*
	Dynamic Memory Load Mode  register to Active Command Time (EMCDynamictMRD - 0x2009 C058)
	The EMCDynamicTMRD register enables you to program the load mode register to active command time, tMRD.
  3:0 Load mode register to active command time (tMRD)
      0x0 - 0x1E n + 1 clock cycles. The delay is in CCLK cycles.
      0xF        16 clock cycles (POR reset value).
	*/
	LPC_EMC->DYNAMICMRD = SDRAM_TMRD;

	/*
	Dynamic Memory Configurati on registers (EMCDy namicConfig0-3 - 0x2009 C100, 120, 140, 160)
	The EMCDynamicConfig0-3 registers enable you to program the configuration information 
  for the relevant dynamic memory chip select . 
  2:0 - Reserved. Read value is undefined, only zero should be written. NA
  4:3 Memory device (MD) 00 SDRAM (POR reset value). 00
      01 Low-power SDRAM.
      1x Reserved.
  6:5 - Reserved. Read value is undefined, only zero should be written. NA
  12:7 Address mapping (AM)
       000000 = reset value.
  13 - Reserved. Read value is undefined, only zero should be written. NA
  14 Address mapping (AM)
     0 = reset value.
  18:15 - Reserved. Read value is undefined, only zero should be written. NA
  19 Buffer enable (B) 
	   0 Buffer disabled for accesses to this chip select (POR reset value)
     1 Buffer enabled for accesses to this chip select.
  20 Write protect (P)
	   0 Writes not protected (POR reset value)
     1 Writes protected.

	cofige para
	4:3  = 00     = SDRAM
	12:7 = 001001	= 128 Mb (8Mx16), 4 banks, row length = 12, column length = 9
	14	 = 1      = 32bit
	0100 0100 1000 0000 
	32bit 128 Mb (8Mx16), 4 banks, row length = 12, column length = 9
	*/
	LPC_EMC->DYNAMICCONFIG0 = 0x0004480;


	/*
	Dynamic Memory Control register (EMCDynamicControl - 0x2009 C020)
  The EMCDynamicControl register controls dynamic memory operation.
 */
	/* General SDRAM Initialization Sequence NOP command*/
	LPC_EMC->DYNAMICCONTROL = 0x0183;
  /* DELAY to allow power and clocks to stabilize ~100 us */
	for(i= 200*40; i;i--);
	/* SDRAM Initialization Sequence PALL command*/
	LPC_EMC->DYNAMICCONTROL = 0x0103;

	/*
  Dynamic Memory  Refresh Timer register  (EMCDynamicRefresh - 0x2009 C024)
	The EMCDynamicRefresh register configures dynamic memory operation.
 */
	LPC_EMC->DYNAMICREFRESH = 2;
	for(i= 256; i; --i); // > 128 clk
	LPC_EMC->DYNAMICREFRESH = P2C(SDRAM_REFRESH) >> 4;

	/* SDRAM Initialization Sequence MODE command */
	LPC_EMC->DYNAMICCONTROL    = 0x00000083;
	//Dummy = *((volatile uint32_t *)(SDRAM_BASE_ADDR | (0x32<<13)));
	
	// NORM
	LPC_EMC->DYNAMICCONTROL = 0x0000;
	LPC_EMC->DYNAMICCONFIG0 |=(1<<19);
	for(i = 100000; i;i--);
}
/*********************************************************************************
**                            End Of File
*********************************************************************************/
