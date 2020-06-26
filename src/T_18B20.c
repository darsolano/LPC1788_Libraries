/*
 * LPCXpresso1769
 T_18B20.c - library for 18B20 Thermometer
 * Created on June 26, 2014, 12:36 AM
 */

#define _1ENABLED
#ifdef _1ENABLED

#include <_1wire.h>
#include <T_18B20.h>
#include <hardware_delay.h>
#include <monitor.h>

Bool DS18B20Init(DS18b20_t* temp)
{

	OW_PinInit(temp->OW_s.port,temp->OW_s.pin);
	if (!OW_reset_pulse()) return FALSE;
	if (DS18B20CmdGetROMIDCode(temp) && DS18B20GetScrachtPadReg(temp) && DS18B20GetTemperature(temp))
	{
		temp->Temperature_s.Lowest_Temp = temp->Temperature_s.Temp_Whole;
		return TRUE;
	}
	return FALSE;
}

Bool DS18B20CmdGetROMIDCode(DS18b20_t* temp)
{
	int loop;
	if (!OW_reset_pulse()) return FALSE;
	OW_write_byte(READ_ROM);
	for (loop = 0; loop < 8; loop++)
	{
		temp->ROMADDR_s.ROMAddressID[loop] = OW_read_byte();
	}
	return TRUE;
}

Bool DS18B20ConfirmIDofDevice(DS18b20_t* temp)
{
	uint8_t i;
	if (!OW_reset_pulse()) return FALSE;
	OW_write_byte(MATCH_ROM); // match ROM

	for (i = 0; i < 8; i++)
	{
		OW_write_byte(temp->ROMADDR_s.ROMAddressID[i]); //send ROM code
		_delay_uS(DELAY_10Us);
	}
	return TRUE;
}
/*
Bool DS18B20WriteAlarm_CfgReg(uint16_t Alarm, uint8_t Config)
{

}
*/
Bool DS18B20GetTemperature(DS18b20_t* temp)
{

	int c16, c2, f10;

	if (!OW_reset_pulse()) return FALSE;

	OW_write_byte(SKIP_ROM); //Skip ROM
	OW_write_byte(CONVERT_T); // Start Conversion
	_delay_uS(5);

	if (!OW_reset_pulse()) return FALSE;

	OW_write_byte(SKIP_ROM); // Skip ROM
	OW_write_byte(READ_SCRATCHPAD); // Read Scratch Pad

	//from scratchpad read, the first 2 byte are from temp register, others are dropped
	temp->ScratchPad_s.TempLow = OW_read_byte();
	temp->ScratchPad_s.TempHi = OW_read_byte();

	if (!OW_reset_pulse()) return FALSE;

	/* Be sure to start the temperature sensor with a temp_type defined */
	/* Start Celsius Convertion*/
	if (temp->Temperature_s.Temp_Type == CELSIUS)
	{
		// Celsius calculation
		temp->Temperature_s.Temp_Whole = (temp->ScratchPad_s.TempHi & 0x07) << 4; // grab lower 3 bits of t1
		temp->Temperature_s.Temp_Whole |= temp->ScratchPad_s.TempLow >> 4; // and upper 4 bits of t0
		temp->Temperature_s.Temp_Decimal = temp->ScratchPad_s.TempLow & 0x0F; // decimals in lower 4 bits of t0
		temp->Temperature_s.Temp_Decimal *= 625; // conversion factor for 12-bit resolution
	}
	/* Start Farenheit convertion */
	if (temp->Temperature_s.Temp_Type == FARENHEIT)
	{
		// Farenheit convertion
		c16 = (temp->ScratchPad_s.TempHi << 8) + temp->ScratchPad_s.TempLow; // result is temp*16, in celcius
		c2 = c16 / 8; // get t*2, with fractional part lost
		f10 = c16 + c2 + 320; // F=1.8C+32, so 10F = 18C+320 = 16C + 2C + 320
		temp->Temperature_s.Temp_Whole = f10 / 10; // get whole part
		temp->Temperature_s.Temp_Decimal = f10 % 10; // get fractional part
	}
	/* Set de high and low temperature reading */
	if (temp->Temperature_s.Temp_Whole >= temp->Temperature_s.Highest_Temp)
		temp->Temperature_s.Highest_Temp = temp->Temperature_s.Temp_Whole;
	if (temp->Temperature_s.Temp_Whole <= temp->Temperature_s.Lowest_Temp)
		temp->Temperature_s.Lowest_Temp = temp->Temperature_s.Temp_Whole;

	return TRUE;
}
/*
void DS18B20RestoreEEPROMValues(void)
{

}

void DS18B20SaveSRAMValues(void)
{

}

POWERTYPE_t DS18B20GetPWRSupplyStatus(void)
{

}

uint8_t DS18B20CalcAndCompareCRC(uint8_t _crc)
{

}
*/
Bool DS18B20GetScrachtPadReg(DS18b20_t* temp)
{
	if (!OW_reset_pulse()) return FALSE;
	OW_write_byte(SKIP_ROM); // Skip ROM
	OW_write_byte(READ_SCRATCHPAD); // Read Scratch Pad

	temp->ScratchPad_s.TempLow 		= OW_read_byte();
	temp->ScratchPad_s.TempHi 		= OW_read_byte();
	temp->ScratchPad_s.UserByte1 	= OW_read_byte();
	temp->ScratchPad_s.UserByte2 	= OW_read_byte();
	temp->ScratchPad_s.Config 		= OW_read_byte();
	temp->ScratchPad_s.reserved0 	= OW_read_byte();
	temp->ScratchPad_s.reserved1 	= OW_read_byte();
	temp->ScratchPad_s.reserved2 	= OW_read_byte();
	temp->ScratchPad_s.CRC 			= OW_read_byte();
	return TRUE;
}

#endif
