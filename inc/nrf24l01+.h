/*
 * wireless.h
 *
 *  Created on: 16/1/2015
 *      Author: dsolano
 */

#ifndef INCLUDE_WIRELESS_H_
#define INCLUDE_WIRELESS_H_

#include <chip.h>
#include <lpc_types.h>
#include <stdint.h>


#define _TX_ADR_WIDTH    			5   // 5 bytes TX(RX) address width
#define _TX_PLOAD_WIDTH  			32  // 16 bytes TX payload


#define IN  0
#define OUT 1

#define rx_mode  0
#define tx_mode  1

#define SPI_STAT_TFE	0x01
#define SPI_STAT_TNF	0x02
#define SPI_STAT_RNE	0x04
#define SPI_STAT_RFF	0x08
#define SPI_STAT_BSY	0x10


/*
 * @author
 * Copyright (C) 2012 Luis R. Hilario http://www.luisdigital.com
 *
 */
#define	_CH 				76			// Channel 0..125
#define	_Address_Width		5			// 3..5
#define _Buffer_Size 		5 			// 1..32

#define _MAX_CHANNEL       127
#define _NUMBER_OF_CHANNELS 128

/**
 * Data Ready RX FIFO interrupt
 */
#define _RX_DR (1<<6)

/**
 * Data Sent TX FIFO interrupt
 */
#define _TX_DS (1<<5)

/**
 * Maximum number of TX retransmits interrupt
 */
#define _MAX_RT (1<<4)

/** Power Down mode
 *
 * Minimal current consumption, SPI can be activated
 *
 * @see NRF24L01_Set_Power(char Mode)
 */
#define _POWER_DOWN	0

/** Power Up mode
 *
 * Standby-I mode
 *
 * @see NRF24L01_Set_Power(char Mode)
 */
#define _POWER_UP	(1<<1)

/** Mode radio transmitter
 *
 * @see NRF24L01_Set_Device_Mode(char Device_Mode)
 * @see NRF24L01_Init(char Device_Mode, char CH, char DataRate,
		char *Address, char Address_Width, char Size_Payload)
 */
#define _TX_MODE	0

/** Mode radio receiver
 *
 * @see NRF24L01_Set_Device_Mode(char Device_Mode)
 * @see NRF24L01_Init(char Device_Mode, char CH, char DataRate,
		char *Address, char Address_Width, char Size_Payload)
 */
#define _RX_MODE	1

/**  Air data rate = 1 Mbps
 *
 *
 * @see NRF24L01_Init(char Device_Mode, char CH, char DataRate,
		char *Address, char Address_Width, char Size_Payload)
 */

/** Air data rate = 2 Mbps
 *
 * @see NRF24L01_Init(char Device_Mode, char CH, char DataRate,
		char *Address, char Address_Width, char Size_Payload)
 */
/**  Air data rate = 1 Mbps
 *
 *
 * @see NRF24L01_Init(char Device_Mode, char CH, char DataRate,
		char *Address, char Address_Width, char Size_Payload)

* Air data rate = 2 Mbps
 *
 * @see NRF24L01_Init(char Device_Mode, char CH, char DataRate,
		char *Address, char Address_Width, char Size_Payload)
 */
typedef enum{
    RF_RATE_250KBPS = 0x20,
    RF_RATE_1MBPS = 0x0,
    RF_RATE_2MBPS = 0x08
}RF_DATA_RATE_e;


/** Enable ShockBurst

 Automatic Retransmission (Up to 1 Re-Transmit on fail of AA)

 Auto Acknowledgment (data pipe 0)

 @see NRF24L01_Set_ShockBurst(char Mode)
 */
#define _ShockBurst_ON 1

/** Disable ShockBurst
 *
 @see NRF24L01_Set_ShockBurst(char Mode)
 */
#define _ShockBurst_OFF 0

/*
 *
 */
typedef enum
{
    RF_PWR_18dBm = 0x00, // must be anded and inverted
    RF_PWR_12dBm = 0x02,
    RF_PWR_6dBm = 0x04,
    RF_PWR_0dBm = 0x06
}RF_PWR_DBM_e;


// REGISTERS
#define CONFIG		0x00
#define EN_AA		0x01
#define EN_RXADDR	0x02
#define SETUP_AW	0x03
#define SETUP_RETR	0x04
#define RF_CH		0x05
#define RF_SETUP	0x06
#define STATUS		0x07
#define OBSERVE_TX	0x08
#define CD			0x09
#define RX_ADDR_P0	0x0A
#define RX_ADDR_P1	0x0B
#define RX_ADDR_P2	0x0C
#define RX_ADDR_P3	0x0D
#define RX_ADDR_P4	0x0E
#define RX_ADDR_P5	0x0F
#define TX_ADDR		0x10
#define RX_PW_P0	0x11
#define RX_PW_P1	0x12
#define RX_PW_P2	0x13
#define RX_PW_P3	0x14
#define RX_PW_P4	0x15
#define RX_PW_P5	0x16
#define FIFO_STATUS	0x17
#define DYNPD		0x1C
#define FEATURE		0x1D

// COMMANDS
#define R_REGISTER			0x00
#define W_REGISTER			0x20
#define R_RX_PAYLOAD        0x61
#define W_TX_PAYLOAD        0xA0
#define FLUSH_TX      		0xE1
#define FLUSH_RX	 		0xE2
#define REUSE_TX_PL  		0xE3
#define ACTIVATE            0x50
#define R_RX_PL_WID         0x60
#define W_ACK_PAYLOAD		0xA8
#define W_TX_PAYLOAD_NOACK	0x58
#define NOP                 0xFF

/* Bit Mnemonics */
#define MASK_RX_DR  6
#define MASK_TX_DS  5
#define MASK_MAX_RT 4
#define EN_CRC      3
#define CRCO        2
#define PWR_UP      1
#define PRIM_RX     0
#define ENAA_P5     5
#define ENAA_P4     4
#define ENAA_P3     3
#define ENAA_P2     2
#define ENAA_P1     1
#define ENAA_P0     0
#define ERX_P5      5
#define ERX_P4      4
#define ERX_P3      3
#define ERX_P2      2
#define ERX_P1      1
#define ERX_P0      0
#define AW          0
#define ARD         4
#define ARC         0
#define PLL_LOCK    4
#define RF_DR       3
#define RF_PWR      6
#define RX_DR       6
#define TX_DS       5
#define MAX_RT      4
#define RX_P_NO     1
#define TX_FULL     0
#define PLOS_CNT    4
#define ARC_CNT     0
#define TX_REUSE    6
#define FIFO_FULL   5
#define TX_EMPTY    4
#define RX_FULL     1
#define RX_EMPTY    0
#define DPL_P5	    5
#define DPL_P4	    4
#define DPL_P3	    3
#define DPL_P2	    2
#define DPL_P1	    1
#define DPL_P0	    0
#define EN_DPL	    2
#define EN_ACK_PAY  1
#define EN_DYN_ACK  0


/* P model bit Mnemonics */
#define RF_DR_LOW   5
#define RF_DR_HIGH  3
#define RF_PWR_LOW  1
#define RF_PWR_HIGH 2

/*
 * This data structure is meant for initialization only
 */
typedef struct nrf24l01p_s
{
    char mode;                    // TX_Mode or RX_Mode
    char addr_size;               // Address size for all pipes and TX pipe
    char *tx_addr;                // Main tx address for the radio, needs to be the same as the rx pipe 0 address
    char *rx_addr_P0;			  // RX address for pipe 0, address width set by SETUP_AW
    char *rx_addr_P1;			  // RX address for pipe 1, address width set by SETUP_AW
    char rx_addr_P2;			  // Last hex digit for address, first four are the same as rx_addr_P1
    char rx_addr_P3;			  // Last hex digit for address, first four are the same as rx_addr_P1
    char rx_addr_P4;			  // Last hex digit for address, first four are the same as rx_addr_P1
    char rx_addr_P5;			  // Last hex digit for address, first four are the same as rx_addr_P1
    char rx_pipe_PW[6];			  // Payload width for refer RX pipe
   char channel;                 // channel 2.4ghz in step of 1Mhz@ 1mbps or less(125 channels) and step of 2Mhz@2mbps(62channels)
    char data_rate;               // data rate or speed in mbps
    char payload_size;            // Size of data buffer to be send or received
    char op_mode;                 // Operation mode as normal or pulled, Shockburst or Enhanced Shockburst
    char power_db;                // RF transmission power in db
}NRF24_t;

typedef struct all_reg_s
{
    char vCONFIG;		//0x00
    char vEN_AA;		//0x01
    char vEN_RXADDR;	//0x02
    char vSETUP_AW;     //0x03
    char vSETUP_RETR;	//0x04
    char vRF_CH;		//0x05
    char vRF_SETUP;     //0x06
    char vSTATUS;		//0x07
    char vOBSERVE_TX;	//0x08
    char vCD;			//0x09
    char vRX_ADDR_P0[5];	//0x0A
    char vRX_ADDR_P1[5];	//0x0B
    char vRX_ADDR_P2;	//0x0C
    char vRX_ADDR_P3;	//0x0D
    char vRX_ADDR_P4;	//0x0E
    char vRX_ADDR_P5;	//0x0F
    char vTX_ADDR[5];		//0x10
    char vRX_PW_P0;	//0x11
    char vRX_PW_P1;	//0x12
    char vRX_PW_P2;	//0x13
    char vRX_PW_P3;	//0x14
    char vRX_PW_P4;	//0x15
    char vRX_PW_P5;	//0x16
    char vFIFO_STATUS;	//0x17
    char vDYNPD;		//0x1C
    char vFEATURE;		//0x1D
}NRF24_REGS_t;


typedef struct stat_s
{
    char STATUS_REG;
    Bool TX_OK;
    Bool RX_RDY;
    Bool TX_RT_FAIL;
    Bool RX_PIPE_NO;
    Bool TX_FIFO_FULL;
}STATUS_REG_s;


/*
 * SPI functions for NRF24L01
 */
 char NRF24L01_ReadReg(char Reg);
 char NRF24L01_WriteReg(char Reg, char Value);
 char NRF24L01_ReadRegBuf(char Reg, char *Buf, int Size);
 char NRF24L01_WriteRegBuf(char Reg, char *Buf, int Size);

/*
 * NRF24L01 functions
 */
 char NRF24L01_Get_Status(void);
 Bool NRF24L01_Get_CD(void);
 Bool NRF24L01_Set_Power(char Mode);
 Bool NRF24L01_Set_CH(char CH);
 void NRF24L01_Set_ShockBurst(char Mode);
 Bool NRF24L01_Set_Address_Width(char Width);
 void NRF24L01_Set_Device_Mode(char Device_Mode);
 void NRF24L01_Set_RX_Pipe(char PipeNum, char* addr, int addr_size, int pld_size);
 void NRF24L01_Disable_All_Pipes(void);
 char NRF24L01_Clear_Interrupts(void);
 void NRF24L01_Set_TX_Address(char *Address, int Size);
 void NRF24L01_Flush_TX(void);
 void NRF24L01_Flush_RX(void);
 void NRF24L01_Init(NRF24_t *radio);
 void NRF24L01_RF_TX(void);
 void NRF24L01_Write_TX_Buf(char *Buf, int Size);
 void NRF24L01_Read_RX_Buf(char *Buf, int Size);
 void NRF24L01_ReadAllData(char *regs);
 void NRF24L01_Set_DataRate_rfPower(RF_DATA_RATE_e data_rate, RF_PWR_DBM_e power_dB);
 void NRF24L01_StartListening(void);
 void NRF24L01_StopListening(void);
 char NRF24L01_Get_FIFO_Status(char *fifo_stat);
 char NRF24L01_Get_Rx_Payload_W(char* pldw);
 char NRF24L01_See_What_Happened(STATUS_REG_s* status_reg);



#endif /* INCLUDE_WIRELESS_H_ */
