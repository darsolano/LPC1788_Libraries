/*
 * nrf24_protocol.c
 *
 *  Created on: Sep 30, 2016
 *      Author: dsolano
 */

#include <nrf24l01+.h>
#include <nrf24_protocol.h>
#include <string.h>
#include <BSP_Waveshare/bsp_waveshare.h>
#include <monitor.h>

//
// Channel info
//
const uint8_t _TX_PAYLOAD[16] = {
		0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
        0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f
};

const uint8_t _TX_ADDR[] =  {0x0e,0x0e,0x0a,0x0a,0x0b};
const uint8_t _RX0_ADDR[] = {0x0e,0x0e,0x0a,0x0a,0x0b};
const uint8_t _RX1_ADDR[] = {0xa5,0xa5,0xa5,0xa5,0x00};

#define CHANNELS		128
uint8_t available_channels[CHANNELS];
NRF24_t rf24;
NRF24_REGS_t *Regs_ptr;
char regs[38];

void nrf24_Start_Protocol(void)
{
	rf24.mode = _RX_MODE;
	rf24.channel = _CH;
	rf24.data_rate = RF_RATE_1MBPS;
	rf24.power_db = RF_PWR_0dBm;
	rf24.tx_addr = (char*)_TX_ADDR;
	rf24.addr_size =_Address_Width;
	rf24.payload_size = _TX_PLOAD_WIDTH;
	rf24.rx_addr_P0 = (char*)_RX0_ADDR;
	rf24.rx_addr_P1 = (char*)_RX1_ADDR;
	rf24.rx_pipe_PW[0] = _TX_PLOAD_WIDTH;
	wsBoard_UART_Init(LPC_UART3);
	xdev_in(wsBoard_UARTGetChar);
	xdev_out(wsBoard_UARTPutChar);

	NRF24L01_Init(&rf24);
}
void nrf24_protocol_Print_Details(void)
{
	NRF24L01_ReadAllData(regs);
	Regs_ptr = (NRF24_REGS_t*) &regs;
	xputs("--------------------------------------------------------\n");
	xputs("nrf24L01+ - 2.4GHz RF Device from Nordic Semiconductors\n");
	xputs("--------------------------------------------------------\n");
	xprintf("CONFIG   :%02X\nTX_ADDR  :%02X.%02X.%02X.%02X.%02X\n", Regs_ptr->vCONFIG, Regs_ptr->vTX_ADDR[0],Regs_ptr->vTX_ADDR[1],
																Regs_ptr->vTX_ADDR[2],Regs_ptr->vTX_ADDR[3],Regs_ptr->vTX_ADDR[4]);

	xprintf("RXP0_ADDR:%02X.%02X.%02X.%02X.%02X\nRXP1_ADDR:%02X.%02X.%02X.%02X.%02X\n",Regs_ptr->vRX_ADDR_P0[0],Regs_ptr->vRX_ADDR_P0[1],
																Regs_ptr->vRX_ADDR_P0[2],Regs_ptr->vRX_ADDR_P0[3],Regs_ptr->vRX_ADDR_P0[4] ,
																Regs_ptr->vRX_ADDR_P1[0],Regs_ptr->vRX_ADDR_P1[1],Regs_ptr->vRX_ADDR_P1[2],
																Regs_ptr->vRX_ADDR_P1[3],Regs_ptr->vRX_ADDR_P1[4]);

	xprintf("RXP2_ADDR:%02X\nRXP3_ADDR:%02X\nRXP4_ADDR:%02X\nRXP5_ADDR:%02X\n", Regs_ptr->vRX_ADDR_P2,Regs_ptr->vRX_ADDR_P3,
																			Regs_ptr->vRX_ADDR_P4,Regs_ptr->vRX_ADDR_P5);
	xprintf("Channel: %d  ", Regs_ptr->vRF_CH);

    if (Regs_ptr->vCONFIG & 0x01){
    	xputs("Mode: PRx  ");
    } else{
    	xputs("Mode: PTx  ");
    }

    // Prints Data Rate for radio
    if (Regs_ptr->vRF_SETUP & _BIT(RF_DR_HIGH))  xputs("2Mbps ");
    else
        if (Regs_ptr->vRF_SETUP & _BIT(RF_DR_LOW)) xputs("250kbps ");
        else
            if (!(Regs_ptr->vRF_SETUP & _BIT(RF_DR_HIGH))) xputs("1Mbps ");

    // Prints Rf power setup for radio
    if (((Regs_ptr->vRF_SETUP&0x06)>>1) == 0) xputs("-18dBm");
    else
        if (((Regs_ptr->vRF_SETUP&0x06)>>1) == 1) xputs("-12dBm");
        else
            if (((Regs_ptr->vRF_SETUP&0x06)>>1) == 2) xputs("-6dBm");
            else
                if (((Regs_ptr->vRF_SETUP&0x06)>>1) == 3) xputs("0dBm");

    xprintf(" RF_SETUP: %02X" , Regs_ptr->vRF_SETUP);
	xprintf("\nRXP0_ADDR_EN:%x\nRXP1_ADDR_EN:%x\nRXP2_ADDR_EN:%x\nRXP3_ADDR_EN:%x\nRXP4_ADDR_EN:%x\nRXP5_ADDR_EN:%x\n",
																(Regs_ptr->vEN_RXADDR&(1<<0)?1:0),
																(Regs_ptr->vEN_RXADDR&(1<<1)?1:0),
																(Regs_ptr->vEN_RXADDR&(1<<2)?1:0),
																(Regs_ptr->vEN_RXADDR&(1<<3)?1:0),
																(Regs_ptr->vEN_RXADDR&(1<<4)?1:0) ,
																(Regs_ptr->vEN_RXADDR&(1<<5)?1:0)
																);
    xprintf("STATUS\t\t: 0x%02X RX_DR:%02X TX_DS:%02X MAX_RT:%02X RX_P_NO:%02X TX_FULL:%02X\n",
               Regs_ptr->vSTATUS,
               (Regs_ptr->vSTATUS & _RX_DR)?1:0,
               (Regs_ptr->vSTATUS & _TX_DS)?1:0,
               (Regs_ptr->vSTATUS & _MAX_RT)?1:0,
               ((Regs_ptr->vSTATUS >> 1) & 0x07),
               (Regs_ptr->vSTATUS & 1)?1:0
              );

    xputs("\nENHANCED SHOCKBURST - MAIN REGISTERS\n");
	xputs("-------------------------------------\n");
	xprintf("EN_AA: %02X  SETUP_RETR: %02X  OBSERVE_TX: %02X\n", Regs_ptr->vEN_AA, Regs_ptr->vSETUP_RETR, Regs_ptr->vOBSERVE_TX);


}

void nrf24_Receive(char* Buf, int len)
{
	NRF24L01_StartListening();

	while ((NRF24L01_Get_Status() & _RX_DR) != _RX_DR);
	NRF24L01_Read_RX_Buf(Buf, len);

	NRF24L01_StopListening();
}


 void nrf24_Send(char* Buf, int len) {
	NRF24L01_Write_TX_Buf(Buf, len);

	NRF24L01_RF_TX();

	while ((NRF24L01_Get_Status() & _TX_DS) != _TX_DS);

	NRF24L01_Clear_Interrupts();
	NRF24L01_Set_Power(_POWER_DOWN);
	NRF24L01_Flush_TX();

}

 // Channel Scanner
#define NUMB_OF_REPS    100
uint8_t values[_NUMBER_OF_CHANNELS];
void nrf24_ChannelScanner(void)
{
    // Go to Standby mode
    NRF24L01_StartListening();
    NRF24L01_StopListening();

// Print out header, high then low digit
    int i = 0;
    while ( i < _NUMBER_OF_CHANNELS )
    {
      xprintf("%x",i>>4);
      ++i;
    }
    xprintf("\n");
    i = 0;
    while ( i < _NUMBER_OF_CHANNELS )
    {
      xprintf("%x",i&0xf);
      ++i;
    }
    xprintf("\n");

    // loop
	while(1)
	{
	  // Clear measurement values
		memset(values,0,_NUMBER_OF_CHANNELS);

		// Scan all channels num_reps times
		int rep_counter = NUMB_OF_REPS;
		while (rep_counter--)
		{
		  int i = _NUMBER_OF_CHANNELS;
		  while (i--)
		  {
			// Select this channel
			NRF24L01_Set_CH(i);

			// Did we get a carrier?
			if ( NRF24L01_Get_CD() )
			  ++values[i];
		  }
		}

		// Print out channel measurements, clamped to a single hex digit
		i = 0;
		while ( i < _NUMBER_OF_CHANNELS )
		{
		  xprintf("%x",MIN(0xf,values[i]&0xf));
		  ++i;
		}
		xprintf("\n");
	}
}
