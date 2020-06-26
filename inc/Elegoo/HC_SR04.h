/*
 * HC_SR04.h
 *
 *  Created on: Dec 26, 2018
 *      Author: dsolano
 */

#ifndef INC_ELEGOO_HC_SR04_H_
#define INC_ELEGOO_HC_SR04_H_

#include <chip.h>

typedef struct{
	int distance_cm;	// per us - microseconds
	int distance_mm;	// per us - microseconds
	int distance_ft;	// per us - microseconds
	int distance_inch;	// per us - microseconds
	int distance_mts;	// per us - microseconds
}HCSR04_DISTANCE_t;


void hcsr04_start(HCSR04_DISTANCE_t* dist, uint8_t trig_port, uint8_t trig_pin, uint8_t ech_port, uint8_t ech_pin);
int hcsr04_Get_Object_Distance(HCSR04_DISTANCE_t* dist);

#endif /* INC_ELEGOO_HC_SR04_H_ */
