/*
 * dht11.h
 *
 *  Created on: Dec 20, 2018
 *      Author: dsolano
 */

#ifndef INC_ELEGOO_DHT11_H_
#define INC_ELEGOO_DHT11_H_

#include <chip.h>

#define DHT11_DEBUG		0
typedef struct{
	uint8_t humidity_entire;
	uint8_t humidity_decimal;
	uint8_t temperature_entire;
	uint8_t temperature_decimal;
	uint8_t chksum;
	uint8_t signal_port;
	uint8_t signal_pin;
	LPC_GPIO_T* pGPIOx;
	Bool valid_data;
	float rel_humidity;
	float temperature;
}DHT11_t;

Bool 	dht11_Init		(DHT11_t* DHTx, uint8_t port, uint8_t pin);
Bool 	dht11_Read_Data	(DHT11_t *DHTx);


#endif /* INC_ELEGOO_DHT11_H_ */
