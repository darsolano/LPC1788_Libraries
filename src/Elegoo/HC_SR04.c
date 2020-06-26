/*
 * HC_SR04.c
 *
 *  Created on: Dec 26, 2018
 *      Author: dsolano
 *      This is a NON-BLOCKIBG library
 */
#include <Elegoo/HC_SR04.h>
#include <hardware_delay.h>
#include <define_pins.h>

#define HCSR04_SOUND_SPEED_METERS_PER_US    .000344						// sound velocity in Meters per microseconds
#define HCSR04_SOUND_SPEED_CM_PER_US	    .0344						// sound velocity in Centimeters per microseconds
#define HCSR04_SOUND_SPEED_MM_PER_US	    .344						// sound velocity in milimeters per microseconds
#define HCSR04_SOUND_SPEED_FEET_PER_US	    .001128						// sound velocity in feets per microseconds
#define HCSR04_SOUND_SPEED_INCH_PER_US	    .013543						// sound velocity in inches per microseconds

#define HCSR04_TIME_MILLIS					1000						// Actually 1000 us = 1ms
#define HCSR04_TRIGGER_TIME					10							// Trigger time is 10us
#define HCSR04_TIMEOUT				    	24*HCSR04_TIME_MILLIS*2		// 24 milliseconds or 4mts by 2 times the sound trip
#define HCSR04_REST_TIME			    	60*HCSR04_TIME_MILLIS		// 24 milliseconds or 4mts
#define HCSR04_MIN_THRESHOLD				116							// 116 us for 2cm minimun reading


uint8_t echo_port, echo_pin, trigger_port, trigger_pin;
int signal_time, distance, time;

DEFINE_PIN(HCSR04_ECHO,echo_port, echo_pin)
DEFINE_PIN(HCSR04_TRIGGER, trigger_port, trigger_pin)

/*
 *  In order to have different reading in different
 *  measurement types (Metrics and British)
 */
static int hcsr04_Calculate_Distance(HCSR04_DISTANCE_t* dist, int time_us){
	dist->distance_mts  = (time_us*HCSR04_SOUND_SPEED_METERS_PER_US)/2;
	dist->distance_cm   = (time_us*HCSR04_SOUND_SPEED_CM_PER_US)/2;
	dist->distance_mm   = (time_us*HCSR04_SOUND_SPEED_MM_PER_US)/2;
	dist->distance_ft   = (time_us*HCSR04_SOUND_SPEED_FEET_PER_US)/2;
	dist->distance_inch = (time_us*HCSR04_SOUND_SPEED_INCH_PER_US)/2;
	return dist->distance_mts;	// divided by 2 because is only 1 round trip not both

}

/*
 * Sent a 10us high signal state to start measuring proximity
 */
static void hcsr04_Send_Trigger_Signal(void){
	HCSR04_TRIGGER_OUTPUT();
	HCSR04_TRIGGER_LOW();
	_delay_ms(4);
	HCSR04_TRIGGER_HIGH();
	_delay_uS(10);
	HCSR04_TRIGGER_LOW();
	_delay_ms(4);
}

/*
 * Get the response echoed from the sensor
 * the resulting time period is the range which
 * the object is from the sensor.
 */
static int hcsr04_Get_Echo_Signal_Duration(void){
	HCSR04_ECHO_INPUT();
	signal_time = 0;
	startTimer0();
	while (HCSR04_ECHO_READ()){
		if (signal_time > HCSR04_TIMEOUT*2) return signal_time;
	}
	signal_time = stopTimer0();
	if (signal_time < HCSR04_MIN_THRESHOLD) return signal_time;
	return signal_time;
}

void hcsr04_start(HCSR04_DISTANCE_t* dist, uint8_t trig_port, uint8_t trig_pin, uint8_t ech_port, uint8_t ech_pin){
	Chip_IOCON_PinMux( LPC_IOCON , trig_port , trig_pin , IOCON_MODE_INACT , IOCON_FUNC0 );
	Chip_IOCON_PinMux( LPC_IOCON , ech_port , ech_pin , IOCON_MODE_INACT , IOCON_FUNC0 );

	trigger_port = trig_port;
	trigger_pin = trig_pin;
	echo_port = ech_port;
	echo_pin = ech_pin;
	HCSR04_TRIGGER_OUTPUT();
	HCSR04_ECHO_INPUT();
}

int hcsr04_Get_Object_Distance(HCSR04_DISTANCE_t* dist){
	hcsr04_Send_Trigger_Signal();
	time = hcsr04_Get_Echo_Signal_Duration();
	if (time == -1) return -1;
	distance = hcsr04_Calculate_Distance(dist, time);
	_delay_uS(HCSR04_REST_TIME);
	return distance;
}
