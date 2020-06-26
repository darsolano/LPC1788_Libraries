/*
 * dht11.c
 *
 *  Created on: Dec 20, 2018
 *      Author: dsolano
 */


#include <Elegoo/dht11.h>
#include <define_pins.h>
#include <string.h>
#include <hardware_delay.h>

#if DHT11_DEBUG
#include <wsboard.h>
#include <monitor.h>
#endif

#define DHT11_GPIO						LPC_GPIO
#define DHT11_PORT_DIR_OUT				1
#define DHT11_PORT_DIR_IN				1

#define DHT11_TIMER_uS					1

#define DHT11_TIMER_INTERVAL_uS			1000000
#define DHT11_TIMER_INTERVAL_mS			1000

#define DHT11_START_SIGNAL_TIME			18000	// 18 millisenconds
#define DHT11_WAIT_TIME					14	// and datasheet says 20-40 microseconds but reads 14
#define DHT11_RESPONSE_SIGNAL_TIME_LOW	85	// microseconds start of response and datasheet says 80us
#define DHT11_RESPONSE_SIGNAL_TIME_HIGH	85	// microseconds end of response and datasheet says 80us
#define DHT11_START_TRANSMISSION_SIGNAL	54	// microseconds and datasheet says 50us
#define DHT11_BIT_HIGH_TIME				70	// microseconds and datasheet says 70
#define DHT11_BIT_LOW_TIME				26	// microseconds and datasheet says 26-28
#define DHT11_BIT_MASK					0x01
#define DHT11_TIMEOUT					100	// micro
#define DHT11_OFFSET					10
#define HIGH							1
#define LOW								0

uint_fast32_t curr_time;
uint8_t bit_read;
uint8_t dht11_port,dht11_pin;	// in scope

DEFINE_PIN(DHT11_SIGNAL,dht11_port,dht11_pin)	// GPIO SIgnal pin functions


static Bool dht11_get_checksum(DHT11_t* DHTx){
	uint16_t chksum;
	chksum = DHTx->humidity_entire 		+
			 DHTx->humidity_decimal 	+
			 DHTx->temperature_entire 	+
			 DHTx->temperature_decimal;
	chksum &= 0xff;

#if DHT11_DEBUG
		xprintf("Checksum : %d \n",chksum);
#endif

	if (chksum == DHTx->chksum) return TRUE;
	else return FALSE;
}

static Bool dht11_read_response(uint8_t maxtime,uint8_t offset, uint8_t xpect_value){
	DHT11_SIGNAL_INPUT();
	curr_time = 0;
	startTimer0();
	if (xpect_value){
		while (DHT11_SIGNAL_READ()){
			if (getTimer0_counter()  > maxtime+offset) break;
		}
	}else{
		while(!DHT11_SIGNAL_READ()){
			if (getTimer0_counter() > maxtime+offset) break;
		}
	}
	curr_time = stopTimer0();
	if ((curr_time < (maxtime-offset)) || (curr_time > (maxtime+offset))){
#if DHT11_DEBUG
		xprintf("Response error...curr_time: %d maxtime: %d @ Expected_value: %d \n",curr_time, maxtime, xpect_value);
#endif
		return 0;
	}
	else{
		return 1;
	}
}

static char dht11_read_data_bits(DHT11_t* DHTx){
	DHT11_SIGNAL_INPUT();

	dht11_read_response(DHT11_START_TRANSMISSION_SIGNAL, 5, LOW);

	// Start reading the bits 70us is a High 24-28us is a low
	curr_time = 0;
	startTimer0();
	while(DHT11_SIGNAL_READ()){
		if (getTimer0_counter() > DHT11_BIT_HIGH_TIME + 5) break;
	}
	curr_time = stopTimer0();

	// Verify the time spam is correct for both high bit and low bit
	if ((curr_time > (DHT11_BIT_HIGH_TIME-5)) && (curr_time < (DHT11_BIT_HIGH_TIME+5))){
		return 1;
	}
	if ((curr_time > (DHT11_BIT_LOW_TIME-5)) && (curr_time < (DHT11_BIT_LOW_TIME+5))){
		return 0;
	}else{
#if DHT11_DEBUG
		xprintf("Read Data Bits error...curr_time: %d \n",curr_time);
#endif
		return 2;
	}
}

static void dht11_read_data_frame(DHT11_t* DHTx){
	char d_bit;
	int byte;
	int bit;
	uint8_t data;
	uint8_t data_all[5];

	DHT11_SIGNAL_OUTPUT();
	curr_time = 0; // Wait 18ms @ low -- Step 1
	startTimer0();
	do{
		DHT11_SIGNAL_LOW();	// Step 1
	}while (getTimer0_counter() < DHT11_START_SIGNAL_TIME);
	curr_time = stopTimer0();
#if DHT11_DEBUG
	xprintf("Step 1...curr_time: %d \n",curr_time);
#endif

	// Wait for 40us as input the pullup will high the pin -- Step 2
	DHT11_SIGNAL_HIGH();
	curr_time = 0;
	DHT11_SIGNAL_INPUT();
	startTimer0();
	do{
		if (getTimer0_counter() > DHT11_WAIT_TIME) break;
	}
	while (DHT11_SIGNAL_READ());
	curr_time = stopTimer0();
#if DHT11_DEBUG
	xprintf("Step 2...curr_time: %d \n",curr_time);
#endif

	// wait from response 80us low and 80us high from DHT11 -- Step 3 --
	dht11_read_response(DHT11_RESPONSE_SIGNAL_TIME_LOW, 5, LOW);

	dht11_read_response(DHT11_RESPONSE_SIGNAL_TIME_HIGH, 5, HIGH);

	// Get all data bit from stream
	for (byte=0;byte<5;byte++){
		for (bit=0;bit<8;bit++){
			d_bit = dht11_read_data_bits(DHTx);
			if (d_bit==0)
				data &= ~(DHT11_BIT_MASK<<(7-bit));	// Remember **Most** significant bit First
			else
			if (d_bit==1)
				data |=  (DHT11_BIT_MASK<<(7-bit));	// Remember **Most** significant bit First
			else {
				DHTx->valid_data = FALSE;
#if DHT11_DEBUG
				xprintf("Read Data frame error...Byte count: %d Bit count: %d Data: %d Bit rcvd: %d\n",byte, bit, data, d_bit);
#endif
				return;
			}
		}
		data_all[byte] = data;
		data = 0;
	}

	DHTx->humidity_entire 		= data_all[0];
	DHTx->humidity_decimal 		= data_all[1];
	DHTx->temperature_entire 	= data_all[2];
	DHTx->temperature_decimal 	= data_all[3];
	DHTx->chksum 				= data_all[4];
	DHTx->rel_humidity = DHTx->humidity_entire + (DHTx->humidity_decimal/100);
	DHTx->temperature = DHTx->temperature_entire + (DHTx->temperature_decimal/100);

#if DHT11_DEBUG
	xprintf("Final ..HEnt%d HDec%d TEnt%d TDec%d CS%d  \n",data_all[0], data_all[1],data_all[2],data_all[3],data_all[4]);
#endif

	// Lets validate Checksum
	if (dht11_get_checksum(DHTx)){
		DHTx->valid_data = TRUE;
		return;
	}else {
		DHTx->valid_data = FALSE;
		return;
	}
}


Bool dht11_Init(DHT11_t* DHTx, uint8_t port, uint8_t pin){
	Chip_IOCON_PinMux( LPC_IOCON , port , pin , IOCON_MODE_INACT , IOCON_FUNC0 );

	DHTx->signal_port = dht11_port = port;
	DHTx->signal_pin = dht11_pin = pin;
	DHTx->pGPIOx = LPC_GPIO;

#if DHT11_DEBUG
	wsBoard_UART_Init(DEBUG_UART);
	wsBoard_Debug_Init();
	xdev_out(Board_UARTPutChar);
	xprintf("LPC1788 LPC OPEN1788 WaveShare Board - DHT11 test app...\n");
#endif
	initTimer0(DHT11_TIMER_INTERVAL_uS);
	dht11_read_data_frame(DHTx);
	if (DHTx->valid_data) return TRUE;
	else return FALSE;
}

Bool dht11_Read_Data(DHT11_t* DHTx){
	DHTx->valid_data = FALSE;
	dht11_read_data_frame(DHTx);
	if (DHTx->valid_data) return TRUE;
	else return FALSE;
}
