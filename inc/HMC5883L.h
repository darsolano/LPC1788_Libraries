/*
 * HMC5883L.h Module GY-271
 *
 *  Created on: Dec 7, 2019
 *      Author: dsolano
 */

#ifndef INC_HMC5883L_H_
#define INC_HMC5883L_H_

#include <chip.h>
#include <lpc_types.h>
#include <stdint.h>

#define HMC5883_I2C_ADDR	0x1E

/*
Address Location Name Access
00 Configuration Register A Read/Write
01 Configuration Register B Read/Write
02 Mode Register Read/Write
03 Data Output X MSB Register Read
04 Data Output X LSB Register Read
05 Data Output Z MSB Register Read
06 Data Output Z LSB Register Read
07 Data Output Y MSB Register Read
08 Data Output Y LSB Register Read
09 Status Register Read
10 Identification Register A Read
11 Identification Register B Read
12 Identification Register C Read
*/
typedef enum{
	CFG_REG_A = 0,
	CFG_REG_B = 1,
	MODE_REG  = 2,
	DATA_OUT_X_MSB = 3,
	DATA_OUT_X_LSB = 4,
	DATA_OUT_Z_MSB = 5,
	DATA_OUT_Z_LSB = 6,
	DATA_OUT_Y_MSB = 7,
	DATA_OUT_Y_LSB = 8,
	STAT_REG = 9,
	_ID_REG_A = 10,
	_ID_REG_B = 11,
	_ID_REG_C = 12
}HMC5883_REG_ADDR_t;

/*
 * Configuration Register A
 * The configuration register is used to configure the device for setting the data output rate and measurement configuration.
 * CRA0 through CRA7 indicate bit locations, with CRA denoting the bits that are in the configuration register. CRA7 denotes
 * the first bit of the data stream. The number in parenthesis indicates the default value of that bit.CRA default is 0x10.
 * CRA7   CRA6   CRA5   CRA4    CRA3    CRA2    CRA1    CRA0
 * (0)   MA1(0) MA0(0) DO2 (1) DO1 (0) DO0 (0) MS1 (0) MS0 (0)
 * Table 3: Configuration Register A
*/

#define CFG_REG_A_MASK				0x7F

#define REG_A_AVG_SAMPLE_MA1_MA0_1	0x00
#define REG_A_AVG_SAMPLE_MA1_MA0_2	0x20
#define REG_A_AVG_SAMPLE_MA1_MA0_4	0x40
#define REG_A_AVG_SAMPLE_MA1_MA0_8	0x60

#define REG_A_DATA_OUT_RATE_0_75HZ	0x00
#define REG_A_DATA_OUT_RATE_1_5HZ	0x04
#define REG_A_DATA_OUT_RATE_3HZ		0x08
#define REG_A_DATA_OUT_RATE_7_5HZ	0x0C
#define REG_A_DATA_OUT_RATE_15HZ	0x10
#define REG_A_DATA_OUT_RATE_30HZ	0x14
#define REG_A_DATA_OUT_RATE_75HZ	0x18

#define REG_A_MEASURE_MODE_NORMAL	0x00
#define REG_A_MEASURE_MODE_POS_BIAS	0x01
#define REG_A_MEASURE_MODE_NEG_BIAS	0x02


/*
 * Configuration Register B
 * The configuration register B for setting the device gain. CRB0 through CRB7 indicate bit locations, with CRB denoting the
 * bits that are in the configuration register. CRB7 denotes the first bit of the data stream. The number in parenthesis
 * indicates the default value of that bit. CRB default is 0x20.
 * CRB7     CRB6    CRB5   CRB4 CRB3 CRB2 CRB1 CRB0
 * GN2 (0) GN1 (0) GN0 (1) (0)   (0)  (0) (0)  (0)
 */
#define CFG_REG_B_MASK				0xE0

#define REG_B_GAIN_CFG_0_88G		0x00
#define REG_B_GAIN_CFG_1_3G			0x20
#define REG_B_GAIN_CFG_1_9G			0x40
#define REG_B_GAIN_CFG_2_5G			0x60
#define REG_B_GAIN_CFG_4G			0x80
#define REG_B_GAIN_CFG_4_7G			0xA0
#define REG_B_GAIN_CFG_5_6G			0xC0
#define REG_B_GAIN_CFG_8_1G			0xE0

/*
 * Mode Register
 * The mode register is an 8-bit register from which data can be read or to which data can be written. This register is used to
 * select the operating mode of the device. MR0 through MR7 indicate bit locations, with MR denoting the bits that are in the
 * mode register. MR7 denotes the first bit of the data stream. The number in parenthesis indicates the default value of that
 * bit. Mode register default is 0x01.
 * MR7   MR6 MR5 MR4 MR3 MR2   MR1    MR0
 * HS(0) (0) (0) (0) (0) (0) MD1 (0) MD0 (1)
*/
#define MODE_REG_MASK				0x83

#define MODE_REG_I2C_HS				0x80
#define MODE_REG_MODE_CONT_OP		0x00
#define MODE_REG_MODE_SING_OP		0x01
#define MODE_REG_MODE_IDLE_OP		0x02
//#define MODE_REG_MODE_IDLE_MEAS		(3<<0)

/*

Data output register
lock.
This bit is set when:
1. some but not all for of the six data output registers have been read,
2. Mode register has been read.

When this bit is set, the six data output registers are locked
and any new data will not be placed in these register until
one of these conditions are met:
1. all six bytes have been read,
2. the mode register is changed,
3. the measurement configuration (CRA) is changed,
4. power is reset.

Ready Bit.
Set when data is written to all six data registers.
Cleared when device initiates a write to the data output
registers and after one or more of the data output registers
are written to. When RDY bit is clear it shall remain cleared
for a 250 μs. DRDY pin can be used as an alternative to
the status register for monitoring the device for
measurement data.
*/

#define STATUS_REG_MASK				0x03
#define STAT_REG_LOCK				0x20
#define STAT_REG_READY				0x01

#define ID_REG_A					0x48
#define ID_REG_B					0x34
#define ID_REG_C					0x33

/*
 * LOW and HIGH Limits according to data sheet
 */
#define HMC5383_LOW_LIMIT_POS_BIAS			273
#define HMC5883_HIGH_LIMIT_POS_BIAS			575
#define HMC5383_LOW_LIMIT_NEG_BIAS			-575
#define HMC5883_HIGH_LIMIT_NEG_BIAS			-243

/*
 * Data Output Register Operation
When one or more of the output registers are read, new data cannot be placed in any of the output data registers until all
six data output registers are read. This requirement also impacts DRDY and RDY, which cannot be cleared until new
data is placed in all the output registers.
*/



typedef struct{
	I2C_ID_T i2cX;
	struct{
		char  cfg_A;		// Use it to config, init, calc,and many more
		char  cfg_B;		// Use it to config, init, calc,and many more
		char  mode;		// Use it to config, init, calc,and many more
		char  status;	// Read Only
		char  _id[3];	// Read Only ID_REG_A, ID_REG_B, ID_REG_C
	}_register;

	struct{
		int16_t  raw_X,		// data from device converted from 2's complement to decimal
				 raw_Y,		// data from device converted from 2's complement to decimal
				 raw_Z;		// data from device converted from 2's complement to decimal
		float    tcomp_X,	// Temperature compensated reading from self test current and first
		 	 	 tcomp_Y,	// Temperature compensated reading from self test current and first
		 	 	 tcomp_Z;	// Temperature compensated reading from self test current and first
	}_data;

	struct{
		int16_t  X_first_read,
			 	 Y_first_read,
				 Z_first_read,
				 X_curr_read,
				 Y_curr_read,
				 Z_curr_read;
	}_SelfTest;

	struct{
		uint8_t  gain;
		float    res_scale;
	}_Gain;

	struct{
		float	dir_degree;
		float	dir_radian;
		float	pointer;
	}_Compass;
}HMC5883L;

/*
 * HMC5883_Initiliaze(HMC5883L*, I2C_ID_T )
 * Init the device with all the parameters for
 * the startup register inside the data structure
 * be sure to ored all the data for each register
 * config registers A and B, mode register.
 * Be sure to set the I2C port to be used
 */
Bool HMC5883_Initiliaze(HMC5883L*, I2C_ID_T );

void HMC5883_Get_Data(HMC5883L* devX);

#endif /* INC_HMC5883L_H_ */
