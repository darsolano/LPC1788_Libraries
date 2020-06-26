/*
 * LPCXpresso LPC1769
 * Author: dsolano
   ADXL335.h - library for ADXL335 Accelerometer
 * Created on June 26, 2014, 12:36 AM
*/

#include <ADXL335.h>
#include <float.h>
#include <hardware_delay.h>
#include <math.h>

#define AXIS(x)			(1<<x)
#define ADCPINMUX		LPC_IOCON

uint32_t* ptr_ADDRx = (uint32_t*)((&ADXL335->DR[0])); // pointer to ADDR0 register in ADC + the channel to be checked


/*
 * Initialization of ADC for ADXL335
 * 	// First lets init tpin to be used
	// ADC0 = Pins 0.23
	// ADC1 = Pins 0.24
	// ADC2 = Pins 0.25
 */
void adxl335_init(void)
{
	ADC_CLOCK_SETUP_T adxl335_cfg;
	// First lets init tpin to be used
	// ADC0 = Pins 0.23	X axis
	// ADC1 = Pins 0.24	Y axis
	// ADC2 = Pins 0.25	Z axis

	Chip_IOCON_PinMux(ADCPINMUX, 0 , 23, IOCON_MODE_REPEATER, IOCON_FUNC1);
	Chip_IOCON_PinMux(ADCPINMUX, 0 , 24, IOCON_MODE_REPEATER, IOCON_FUNC1);
	Chip_IOCON_PinMux(ADCPINMUX, 0 , 25, IOCON_MODE_REPEATER, IOCON_FUNC1);

	/*ADC Init */
	adxl335_cfg.adcRate = ADC_MAX_SAMPLE_RATE;
	Chip_ADC_Init(ADXL335 , &adxl335_cfg);
	Chip_ADC_SetStartMode(ADXL335 , ADC_NO_START ,ADC_TRIGGERMODE_RISING );
	Chip_ADC_EnableChannel(ADXL335, ADC_CH0, ENABLE);
	Chip_ADC_EnableChannel(ADXL335, ADC_CH1, ENABLE);
	Chip_ADC_EnableChannel(ADXL335, ADC_CH2, ENABLE);

}

/*
 * Read from ADC each axis at a time from AD0 thru AD2
 * store reading in data structure ACC_DATA_t
 */
void adxl335_ReadAxis(void)
{
	int i = 0;
	int result=0;
	ADXL335->CR |= ADC_CR_BURST;// Start Burst conversion for the channel
	for (i=0;i<3;i++)
	{
		int count = SAMPLE_RATE;
		result = 0;

		ADXL335->CR |= ADC_CR_CH_SEL(i);// Enable the channel for conversion
		while (count--) // read 16 time for accuracy
		{
			_delay_uS(70);
			result += ADC_DR_RESULT(*(ptr_ADDRx+i));	// read and add 16 times the result to get the media
		}
		// get the axis reading from ADC 0 thru 2
		if (i==0) acc.Xread = result/SAMPLE_RATE; // save result per Axis X
		if (i==1) acc.Yread = result/SAMPLE_RATE; // save result per Axis Y
		if (i==2) acc.Zread = result/SAMPLE_RATE; // save result per Axis Z
		ADXL335->CR &= ~ADC_CR_CH_SEL(i);// Disable the channel
	}
	// Shutdown ADC converter
	ADXL335->CR &= ~ADC_CR_BURST;// Stop Burst conversion for the channel

	//convert to voltage readings @ 0g conversions for each axis
	acc.Xvolt = (Vx(acc.Xread)) - X_0g;
	acc.Yvolt = (Vy(acc.Yread)) - Y_0g;
	acc.Zvolt = (Vz(acc.Zread)) - Z_0g;

	// Convert to g = 9.8m/s^2
	acc.Xg = acc.Xvolt / X_SENS;
	acc.Yg = acc.Yvolt / Y_SENS;
	acc.Zg = acc.Zvolt / Z_SENS;

	// Get Resulting Vector
	acc.Rvector = sqrt((pow(acc.Xg , 2) +
						pow(acc.Yg , 2) +
						pow(acc.Zg , 2))); // 3D Pythagorean Theorem

	// Get the inclination angle relative to R vector
	acc.Xangle = acos(acc.Xg / acc.Rvector);
	acc.Yangle = acos(acc.Yg / acc.Rvector);
	acc.Zangle = acos(acc.Zg / acc.Rvector);

	// Direction Cosine
	acc.Dir_Cosine = sqrt(	pow(cos(acc.Xg / acc.Rvector) , 2) +
							pow(cos(acc.Yg / acc.Rvector) , 2) +
							pow(cos(acc.Zg / acc.Rvector) , 2) );

	return;
}

