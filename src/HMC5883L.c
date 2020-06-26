/*
 * HMC5882L.c Module GY-271
 *
 *  Created on: Dec 7, 2019
 *      Author: dsolano
 */

#include <HMC5883L.h>
#include <hardware_delay.h>
#include <math.h>

uint8_t buff[16] = {0};

static Bool     hmc5883_chip_exist				(I2C_ID_T idx);
static Bool	    hmc5883_write_register			(HMC5883L* devX, HMC5883_REG_ADDR_t addr, uint8_t data);
static Bool     hmc5883_read_register 			(HMC5883L* devX, HMC5883_REG_ADDR_t addr,char* buffer, int len);
static Bool     hmc5883_IsReady  	 			(HMC5883L* devX);
static Bool     hmc5883_verify_id	 			(HMC5883L* devX);
static void 	hmc5883_calc_scale_factor		(HMC5883L* devX, uint8_t gain);
static void     hmc5883_self_test				(HMC5883L* devX);
static Bool 	hmc5883_get_axis_data			(HMC5883L* devX);

static Bool     hmc5883_chip_exist(I2C_ID_T idx){
	Bool result;
	result = Chip_I2C_MasterSend(idx, HMC5883_I2C_ADDR, buff, 1);
	return result;
}


static Bool hmc5883_write_register(HMC5883L* devX, HMC5883_REG_ADDR_t addr, uint8_t data){
	int8_t status;
	buff[0] = addr;
	buff[1] = data;
	status = Chip_I2C_MasterSend(devX->i2cX, HMC5883_I2C_ADDR, buff, 2);
	return (Bool) status;
}

static Bool hmc5883_read_register (HMC5883L* devX, HMC5883_REG_ADDR_t addr, char* buffer, int len){

	if(Chip_I2C_MasterCmdRead(devX->i2cX, HMC5883_I2C_ADDR, addr, (uint8_t*)buffer, len) == len) return SUCCESS;
	else return ERROR;
}

static Bool hmc5883_IsReady(HMC5883L* devX){
	char stat_r;
	if (hmc5883_read_register(devX, STAT_REG, &stat_r, 1) & STATUS_REG_MASK)
		return (stat_r && STAT_REG_READY);
	else return 0;
}

static Bool hmc5883_verify_id(HMC5883L* devX){

	devX->_register._id[0] = 0;
	devX->_register._id[1] = 0;
	devX->_register._id[2] = 0;

	hmc5883_read_register(devX, _ID_REG_A, &devX->_register._id[0], 3);

	if (devX->_register._id[0] != ID_REG_A) return ERROR;
	if (devX->_register._id[1] != ID_REG_B) return ERROR;
	if (devX->_register._id[2] != ID_REG_C) return ERROR;

	return SUCCESS;
}

static void hmc5883_calc_scale_factor(HMC5883L* devX, uint8_t gain){

	devX->_Gain.gain = gain;

	switch (gain) {
		case REG_B_GAIN_CFG_0_88G:
			devX->_Gain.res_scale = .73/1000;
			break;
		case REG_B_GAIN_CFG_1_3G:
			devX->_Gain.res_scale = .92/1000;

			break;
		case REG_B_GAIN_CFG_1_9G:
			devX->_Gain.res_scale = 1.22/1000;

			break;
		case REG_B_GAIN_CFG_2_5G:
			devX->_Gain.res_scale = 1.52/1000;

			break;
		case REG_B_GAIN_CFG_4G:
			devX->_Gain.res_scale = 2.27/1000;

			break;
		case REG_B_GAIN_CFG_4_7G:
			devX->_Gain.res_scale = 2.56/1000;

			break;
		case REG_B_GAIN_CFG_5_6G:
			devX->_Gain.res_scale = 3.03/1000;

			break;
		case REG_B_GAIN_CFG_8_1G:
			devX->_Gain.res_scale = 4.35/1000;

			break;
			default:
			break;
	}

}

static void hmc5883_self_test(HMC5883L* devX){

	int16_t low_limit, high_limit;
	char save_data[3] = {0};

	/* read actual data form register to restore the value at the end*/
	hmc5883_read_register(devX, CFG_REG_A, save_data, 3);	// save all 3 registers CRA, CRB and MODE

	devX->_register.cfg_A = (REG_A_AVG_SAMPLE_MA1_MA0_8 | REG_A_DATA_OUT_RATE_15HZ | REG_A_MEASURE_MODE_POS_BIAS);
	hmc5883_write_register(devX, CFG_REG_A, devX->_register.cfg_A);
	devX->_register.cfg_B = REG_B_GAIN_CFG_4_7G;
	hmc5883_write_register(devX, CFG_REG_B, devX->_register.cfg_B);
	devX->_register.mode = MODE_REG_MODE_CONT_OP;
	hmc5883_write_register(devX, MODE_REG, devX->_register.mode);

	low_limit = HMC5383_LOW_LIMIT_POS_BIAS;
	high_limit = HMC5883_HIGH_LIMIT_POS_BIAS;

	while (!hmc5883_IsReady(devX));	/*Wait for device to be ready*/

	if (!hmc5883_get_axis_data(devX)) return;

	while (1){
		//Verify that the output data is within reasonable boundaries for gain = 5, positive bias
		if ((devX->_data.raw_X > low_limit) || (devX->_data.raw_X < high_limit) ||
			(devX->_data.raw_Y > low_limit) || (devX->_data.raw_Y < high_limit) ||
			(devX->_data.raw_Z > low_limit) || (devX->_data.raw_Z < high_limit))

			/* Error has occurred with at least one of the axis*/
		{
			if (devX->_register.cfg_B < REG_B_GAIN_CFG_8_1G)
				hmc5883_write_register(devX, CFG_REG_B,devX->_register.cfg_B + 0x20);	//adds one to gain until gain reach 7 or 8.1gauss




			continue;
		}
	}
}

static Bool hmc5883_get_axis_data(HMC5883L* devX){
	char raw_data[6] = {0};

	while (!hmc5883_IsReady(devX));	// verify that the device is ready to be read on axis

	// read all 6 data registers
	if (!hmc5883_read_register(devX, DATA_OUT_X_MSB, raw_data, 6))
		return ERROR;

	// Get the 16bit data in variables
	devX->_data.raw_X = raw_data[0]<<8 | raw_data[1];
	devX->_data.raw_Y = raw_data[2]<<8 | raw_data[2];
	devX->_data.raw_Z = raw_data[4]<<8 | raw_data[5];
	return SUCCESS;
}

/*
 * PUBLIC FUNCTIONS
 */
Bool HMC5883_Initiliaze(HMC5883L* devX, I2C_ID_T i2cid){
	devX->i2cX = i2cid;
	if (!hmc5883_chip_exist(i2cid)) return ERROR;	/*Check if chip is connected*/
	if (!hmc5883_verify_id(devX)) return ERROR; 				/* Verify ID register*/

	/* Write the data to configuration registers*/
	if (!hmc5883_write_register(devX, CFG_REG_A, devX->_register.cfg_A) ||
		!hmc5883_write_register(devX, CFG_REG_B, devX->_register.cfg_B) ||
		/* Write data mode to Mode register*/
		!hmc5883_write_register(devX, MODE_REG, devX->_register.mode))
		return ERROR;

	while (!hmc5883_IsReady(devX));	/* Wait for the device to be ready*/

	hmc5883_self_test(devX);	// do a self test to compensate the readings

	devX->_register.mode = MODE_REG_MODE_IDLE_OP;	// Idle operation

	return SUCCESS;
}



void HMC5883_Get_Data(HMC5883L* devX){
	devX->_register.mode = MODE_REG_MODE_SING_OP;
	hmc5883_write_register(devX, MODE_REG, devX->_register.mode);
	while (!hmc5883_IsReady(devX));	/* Wait for the device to be ready*/
	hmc5883_get_axis_data(devX);	// read twice to discard first attempt to read axis, just in case
	hmc5883_get_axis_data(devX);
	devX->_register.mode = MODE_REG_MODE_IDLE_OP;	// Idle operation

}

