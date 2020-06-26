/*
 * wireless.c with nrf24l01+
 *
 *  Created on: 16/1/2015
 *      Author: dsolano
 */

#include <string.h>
#include <define_pins.h>
#include <nrf24l01+.h>
#include <BSP_Waveshare/bsp_waveshare.h>
#include <hardware_delay.h>

#define RF24L01p	LPC_SSP0

	 //* CE PIN 2.0
	 //* CSN PIN 0.2

DEFINE_PIN(RADIO,2,10)
DEFINE_PIN(CSN,0,2)



/****************************************************************
 * RADIO BASIC LOW LEVEL FUNCTIONS - ALL PRIVATE
 ****************************************************************/

void NRF24L01_SSP_INIT(void)
{
//    Board_SSP_Init(RF24L01p);
//    Chip_SSP_Init(RF24L01p);	// Master, 8bits
//    Chip_SSP_SetBitRate(RF24L01p, 5000000);
}


uint8_t spirw(LPC_SSP_T* SSPdev, uint8_t data)
{
	while (!(SSPdev->SR & SSP_STAT_TFE));
	SSPdev->DR = data;		// send a byte
	while (!(SSPdev->SR & SSP_STAT_RNE));
	return SSPdev->DR;		// Receive a byte
}
/**
 Read a register

 @param Reg Register to read

 @return Registry Value
*/
 char NRF24L01_ReadReg(char Reg) {
	char Result;

	CSN_LOW();
	spirw(RF24L01p, Reg);
	Result = spirw(RF24L01p, 0);
	CSN_HIGH();
	return Result;
}

/**
 Returns the STATUS register and then modify a register

 @param Reg Register to change
 @param Value New value

 @return STATUS Register
*/
 char NRF24L01_WriteReg(char Reg, char Value) {
	char Result;

	CSN_LOW();
	Result = spirw(RF24L01p, Reg);
	spirw(RF24L01p, Value);
	CSN_HIGH();

	return Result;
}

/**
 Returns the STATUS register and then read "n" registers

 @param Reg Register to read
 @param Buf Pointer to a buffer
 @param Size Buffer Size

 @return STATUS Register
*/
 char NRF24L01_ReadRegBuf(char Reg, char *Buf, int Size) {
	int i;
	char Result;

	CSN_LOW();
	Result = spirw(RF24L01p, Reg);

	for (i = 0; i < Size; i++) {
		Buf[i] = spirw(RF24L01p,0);
	}

	CSN_HIGH();

	return Result;
}

/**
 Returns the STATUS register and then write "n" registers

 @param Reg Registers to change
 @param Buf Pointer to a buffer
 @param Size Buffer Size

 @return STATUS Register
*/
 char NRF24L01_WriteRegBuf(char Reg, char *Buf, int Size) {
	int i;
	char Result;

	CSN_LOW();
	Result = spirw(RF24L01p,Reg);

	for (i = 0; i < Size; i++) {
		spirw(RF24L01p, Buf[i]);
	}
	CSN_HIGH();

	return Result;
}

/**
 Returns the STATUS register

 @return STATUS Register
*/
 char NRF24L01_Get_Status(void) {
	char Result;

	CSN_LOW();
	Result = spirw(RF24L01p, 0);
	CSN_HIGH();

	return Result;
}

/**
 Returns the carrier signal in RX mode (high when detected)
 only is used when in RX mode.
 @return CD
*/
 Bool NRF24L01_Get_CD(void) {

	 NRF24L01_StartListening();
	 _delay_uS(130);	// as per manual and indications
	 NRF24L01_StopListening();
	 if (NRF24L01_ReadReg(CD))
	 {
		return 1;
	 }
	 else
	 {
		return 0;
	 }
}

/**
 Select power mode

 @param Mode = _POWER_DOWN, _POWER_UP

 @see _POWER_DOWN
 @see _POWER_UP

*/
 Bool NRF24L01_Set_Power(char Mode) {
	char cfg;

	cfg = NRF24L01_ReadReg(CONFIG) & 0b11111101; // Read Conf. Reg. AND Clear bit 1 (PWR_UP) and 7 (Reserved)
	NRF24L01_WriteReg(W_REGISTER | CONFIG, cfg|Mode);

	// Check if written values were correct
	if (NRF24L01_ReadReg(CONFIG) == (cfg|Mode)) return TRUE;
	else return FALSE;
}

/**
 Select the radio channel

 @param CH = 0..125

*/
 Bool NRF24L01_Set_CH(char CH) {
	NRF24L01_WriteReg(W_REGISTER | RF_CH, (CH & 0x7f)); // Clear bit 8

	if ((NRF24L01_ReadReg(RF_CH) & 0x7f) == CH) return TRUE;
	else return FALSE;
}

/**
 Select Enhanced ShockBurst ON/OFF

 Disable this functionality to be compatible with nRF2401

 @param Mode = _ShockBurst_ON, _ShockBurst_OFF

 @see _ShockBurst_ON
 @see _ShockBurst_OFF

*/
 void NRF24L01_Set_ShockBurst(char Mode) {
	NRF24L01_WriteReg(W_REGISTER | SETUP_RETR, Mode);
	NRF24L01_WriteReg(W_REGISTER | EN_AA, Mode);
}

/**
 Select the address width

 @param Width = 3..5
*/
 Bool NRF24L01_Set_Address_Width(char Width) {
	 char aw = Width & 0b00000011;
	NRF24L01_WriteReg(W_REGISTER | SETUP_AW, (Width & 3)-2);
	if ((NRF24L01_ReadReg(SETUP_AW) & 0x03) == aw) return TRUE;
	else return FALSE;
}


/**
 Enables and configures the pipe receiving the data

 @param PipeNum Number of pipe
 @param Address Address
 @param AddressSize Address size
 @param PayloadSize Buffer size, data receiver

*/
 void NRF24L01_Set_RX_Pipe(char PipeNum, char* addr, int addr_size, int pld_size) {

	NRF24L01_WriteReg(W_REGISTER | EN_RXADDR, NRF24L01_ReadReg(EN_RXADDR) | (1 << PipeNum));	// enable the Pipe number
	NRF24L01_WriteRegBuf(W_REGISTER | (RX_ADDR_P0 + PipeNum), addr, addr_size);
	NRF24L01_WriteReg(W_REGISTER | (RX_PW_P0 + PipeNum), pld_size);
}

/**
 Disable all pipes
*/
 void NRF24L01_Disable_All_Pipes(void) {
	NRF24L01_WriteReg(W_REGISTER | EN_RXADDR, 0);
}


/** Returns the STATUS register and then clear all interrupts
 *
 * @return STATUS Register
 */
 char NRF24L01_Clear_Interrupts(void) {
	return NRF24L01_WriteReg(W_REGISTER | STATUS, _RX_DR | _TX_DS | _MAX_RT);
}

/**
 Sets the direction of transmission

 @param Address Address
 @param Size Address size 3..5

*/
 void NRF24L01_Set_TX_Address(char *Address, int Size) {
	NRF24L01_WriteRegBuf(W_REGISTER | TX_ADDR, Address, Size);
}

/**
 Empty the transmit buffer

*/
 void NRF24L01_Flush_TX(void) {
	CSN_LOW();
	spirw(RF24L01p,FLUSH_TX);
	CSN_HIGH();
}

/**
 Empty the receive buffer
*/
 void NRF24L01_Flush_RX(void) {
	CSN_LOW();
	spirw(RF24L01p, FLUSH_RX);
	CSN_HIGH();
}

void NRF24L01_TX_Reuse(void)
{
	RADIO_ON();
	_delay_uS(10);
	NRF24L01_WriteReg(REUSE_TX_PL,0);
}

 char NRF24L01_Get_FIFO_Status(char *fifo_stat)
 {
	char status;
	CSN_LOW();
	status = spirw(RF24L01p, FIFO_STATUS);
	*fifo_stat = spirw(RF24L01p, 0);
	CSN_HIGH();
    return status;
 }

/*
 * Read RX Payload width from top of RX FIFO
 */
char NRF24L01_Get_Rx_Payload_W(char* pldw)
{
	char status;
	CSN_LOW();
	status = spirw(RF24L01p, R_RX_PL_WID);
	*pldw = spirw(RF24L01p, 0);
	CSN_HIGH();
	return status;
}

/*
 * See what happened with the last action in total
 * for TX and RX packets
 */
char NRF24L01_See_What_Happened(STATUS_REG_s* status_reg)
{
    status_reg->STATUS_REG 		= NRF24L01_Get_Status();
    status_reg->RX_PIPE_NO 		= status_reg->STATUS_REG & (7<<RX_P_NO) ? TRUE:FALSE;
    status_reg->RX_RDY 			= status_reg->STATUS_REG & _RX_DR ? TRUE:FALSE;
    status_reg->TX_FIFO_FULL 	= status_reg->STATUS_REG & (1<<TX_FULL) ? TRUE:FALSE;
    status_reg->TX_OK 			= status_reg->STATUS_REG & _TX_DS ? TRUE:FALSE;
    status_reg->TX_RT_FAIL 		= status_reg->STATUS_REG & _MAX_RT ? TRUE:FALSE;
    return status_reg->STATUS_REG;
}


/**
 Select mode receiver or transmitter

 @param Device_Mode = _TX_MODE, _RX_MODE

 @see _TX_MODE
 @see _RX_MODE
 */
 void NRF24L01_Set_Device_Mode(char Device_Mode)
{
    char cfg;
    cfg = NRF24L01_ReadReg(CONFIG) & 0b11111110; // Read Conf. Reg. AND Clear bit 0 (PRIM_RX) and 7 (Reserved)
    NRF24L01_WriteReg(W_REGISTER | CONFIG, cfg | Device_Mode );
}


/*
 * Configure On the Air Data Rate and RF Wave Power
 */

 void NRF24L01_Set_DataRate_rfPower(RF_DATA_RATE_e data_rate, RF_PWR_DBM_e power_dB)
{
    uint8_t rf_reg;

    // Set data Rate
    rf_reg = NRF24L01_ReadReg(RF_SETUP) & 0b00000110;
	NRF24L01_WriteReg(W_REGISTER | RF_SETUP , rf_reg | data_rate);

	// Set RF_Power
    rf_reg = NRF24L01_ReadReg(RF_SETUP) & 0b00101000;
    NRF24L01_WriteReg(W_REGISTER | RF_SETUP , rf_reg|power_dB);
}

/**
 Initializes the device
 @param Device_Mode = _TX_MODE, _RX_MODE
 @param CH = 0..125
 @param DataRate = _1Mbps, _2Mbps
 @param Address Address
 @param Address_Width Width direction: 3..5
 @param Size_Payload Data buffer size

 @see _TX_MODE
 @see _RX_MODE
 @see _1Mbps
 @see _2Mbps
*/
 void NRF24L01_Init(NRF24_t *radio)
{

    NRF24L01_SSP_INIT();
    RADIO_OUTPUT();
    CSN_OUTPUT();

    NRF24L01_WriteReg(W_REGISTER|CONFIG , 0 );	// Clear config register
    // Disable Enhanced ShockBurst
    NRF24L01_Set_ShockBurst(_ShockBurst_OFF);

    // RF output power in TX mode = 0dBm (Max.)
    // Set LNA gain
    NRF24L01_Set_DataRate_rfPower(radio->data_rate , radio->power_db);
    NRF24L01_Set_Address_Width(radio->addr_size);

    NRF24L01_Set_RX_Pipe(0, radio->rx_addr_P0,radio->addr_size,radio->payload_size);
    //NRF24L01_Set_RX_Pipe(1, radio);

    NRF24L01_Set_CH(radio->channel);

    NRF24L01_Set_TX_Address(radio->tx_addr, radio->addr_size); // Set Transmit address

    // Bits 4..6: Reflect interrupts as active low on the IRQ pin
	// Bit 3: Enable CRC
	// Bit 2: CRC 1 Byte
	// Bit 1: Power Up
    // Bit 0: TX or RX mode
    NRF24L01_WriteReg(W_REGISTER | CONFIG, 0b00001010 | radio->mode);
	_delay_uS(1500);
	RADIO_OFF();
}

/**
 Turn on transmitter, and transmits the data loaded into the buffer
*/
void NRF24L01_RF_TX(void)
{
	NRF24L01_Set_Power(_POWER_UP);
	NRF24L01_Set_Device_Mode(_TX_MODE);
	_delay_uS(150);
	RADIO_OFF();
	RADIO_ON();
	_delay_uS(15);
	RADIO_OFF();
}

/**
 Writes the buffer of data transmission

 @param Buf Buffer with data to send
 @param Size Buffer size

*/
 void NRF24L01_Write_TX_Buf(char *Buf, int Size) {
	NRF24L01_WriteRegBuf( W_TX_PAYLOAD, Buf, Size);
}

/**
 Read the data reception buffer

 @param Buf Buffer with data received
 @param Size Buffer size

*/
 void NRF24L01_Read_RX_Buf(char *Buf, int Size) {
	NRF24L01_ReadRegBuf(R_RX_PAYLOAD, Buf, Size);
}

 void NRF24L01_StartListening(void){
     NRF24L01_Clear_Interrupts();
     NRF24L01_Flush_RX();
     NRF24L01_Flush_TX();
     NRF24L01_Set_Power(_POWER_UP);
     NRF24L01_Set_Device_Mode(_RX_MODE);
     RADIO_ON();
     _delay_uS(1500);
 }

 void NRF24L01_StopListening(void)
 {
	RADIO_OFF();
	NRF24L01_Flush_RX();
	NRF24L01_Flush_TX();
	NRF24L01_Clear_Interrupts();
	NRF24L01_Set_Power(_POWER_DOWN);

 }


/**
 read all register from nrf24l01+
 @param NRF24_REGS_t data structure

 @see typedef struct all_reg_s
{
    UCHAR8 vCONFIG;		//0x00
    UCHAR8 vEN_AA;		//0x01
    UCHAR8 vEN_RXADDR;	//0x02
    UCHAR8 vSETUP_AW;     //0x03
    UCHAR8 vSETUP_RETR;	//0x04
    UCHAR8 vRF_CH;		//0x05
    UCHAR8 vRF_SETUP;     //0x06
    UCHAR8 vSTATUS;		//0x07
    UCHAR8 vOBSERVE_TX;	//0x08
    UCHAR8 vCD;			//0x09
    UCHAR8 vRX_ADDR_P0[5];	//0x0A
    UCHAR8 vRX_ADDR_P1[5];	//0x0B
    UCHAR8 vRX_ADDR_P2;	//0x0C
    UCHAR8 vRX_ADDR_P3;	//0x0D
    UCHAR8 vRX_ADDR_P4;	//0x0E
    UCHAR8 vRX_ADDR_P5;	//0x0F
    UCHAR8 vTX_ADDR[5];		//0x10
    UCHAR8 vRX_PW_P0;	//0x11
    UCHAR8 vRX_PW_P1;	//0x12
    UCHAR8 vRX_PW_P2;	//0x13
    UCHAR8 vRX_PW_P3;	//0x14
    UCHAR8 vRX_PW_P4;	//0x15
    UCHAR8 vRX_PW_P5;	//0x16
    UCHAR8 vFIFO_STATUS;	//0x17
    UCHAR8 vDYNPD;		//0x1C
    UCHAR8 vFEATURE;		//0x1D
}NRF24_REGS_t;
 * Must declare a 38 byte data buffer in order to hold all the data then
 * cast the data buffer as the NRF24_REGS_t data structure.
 * Good Luck!!! DS
 */
 void NRF24L01_ReadAllData(char *regs)
{
    unsigned int outer;
    unsigned int inner;
    unsigned int dataloc = 0;
    char buffer[5];

    for (outer = 0; outer <= 0x1E; outer++)
    {
        if (outer >= 0x18 && outer <= 0x1B) continue;
        NRF24L01_ReadRegBuf(outer, buffer, 5);

        for (inner = 0; inner < 5; inner++)
        {
            if (inner >= 1 && (outer != 0x0A && outer != 0x0B && outer != 0x10))
                break;

            regs[dataloc] = buffer[inner];
            dataloc++;
        }
    }
}
