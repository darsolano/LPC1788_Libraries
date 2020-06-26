/* 
 * File:   ADXL335.h
 * Author: dsolano
 * LPCXpresso LPC1769
   ADXL335.h - library for ADXL335 Accelerometer
 * Created on June 27, 2014, 6:39 PM
 */

#ifndef ADXL335_H
#define	ADXL335_H

#ifdef	__cplusplus
extern "C" {
#endif

//#define INT

#include <chip.h>
#include <lpc_types.h>



#define ADXL335			LPC_ADC
#define ADC_RATE		200000

/*
 * Used to point to ADC address register in MCU
 */
#define X_AXIS			0
#define Y_AXIS			1
#define Z_AXIS			2

/*
 * Times that we sample the data from ADC
 * in order to get an accurate reading
 */
#define SAMPLE_RATE		16

/*
 * Constants reference from MCU configuration
 */
#define ADC_VREF_MAX		3.0
#define MAX_CONV			4095


#define Vx(x)				(x*ADC_VREF_MAX)/MAX_CONV
#define Vy(y)				(y*ADC_VREF_MAX)/MAX_CONV
#define Vz(z)				(z*ADC_VREF_MAX)/MAX_CONV

/*
 * Sensitivity from Datasheet
 */
#define X_SENS				.295	//.330mV/g -----> g = 9.8m/s^2
#define Y_SENS				.295	//.330mV/g -----> g = 9.8m/s^2
#define Z_SENS				.295	//.330mV/g -----> g = 9.8m/s^2

/*
 * Zero g reference from Datasheet
 */
#define X_0g				(ADC_VREF_MAX / 2)
#define Y_0g				(ADC_VREF_MAX / 2)
#define Z_0g				(ADC_VREF_MAX / 2)

typedef struct{
	uint32_t Xread;
	uint32_t Yread;
	uint32_t Zread;
	float Xvolt;
	float Yvolt;
	float Zvolt;
	float Xg;
	float Yg;
	float Zg;
	float Xangle;
	float Yangle;
	float Zangle;
	float Rvector;
	float Dir_Cosine;
}ACC_DATA_t;
ACC_DATA_t acc;


/*
 * Initialization of ADC for ADXL335
 * 	// First lets init pin to be used
	// ADC0 = Pins 0.23
	// ADC1 = Pins 0.24
	// ADC2 = Pins 0.25
 */
void 		adxl335_init(void);

/*
 * Read from ADC each axis at a time from AD0 thru AD2
 * store reading in data structure ACC_DATA_t
 */
void adxl335_ReadAxis(void);

#ifdef	__cplusplus
}
#endif

#endif	/* ADXL335_H */

