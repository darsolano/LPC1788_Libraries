/* 
 * File:   T_18B20.h
 * Author: dsolano
 *
 * Created on June 27, 2014, 6:42 PM
 */

#ifndef T_18B20_H
#define	T_18B20_H

#define _1ENABLED
#ifdef	__cplusplus
extern "C" {
#endif

#ifdef _1ENABLED

// Commands for DS18B20 Dallas Semiconductor 18B20
// ROM Commands
#define SEARCH_ROM          0xF0    //Verify existance of other Devices on multislave network
#define READ_ROM            0x33    //Get ROM Code out of Device
#define MATCH_ROM           0x55    //ID device with the address collected
#define SKIP_ROM            0xCC
#define ALARM_SEARCH        0xEC

//Function Set Commands
#define CONVERT_T           0x44    // Start Temperature convertions
#define WRITE_SCRATCHPAD    0x4E    // Writes USer Byte and Config Register
#define READ_SCRATCHPAD     0xBE    // Read all 9 Bytes from memory on DS18B20
#define COPY_SCRATCHPAD     0x48    // Copy Memory to EEPROM on DS18B20
#define RECALL_E2           0xB8    // Restore EEPROM values to memory
#define READ_POWER_SUPPLY   0xB4    // Get the Power Supply status on DS18B20

/*Temperature type currently stored in struct data*/
typedef enum {
    CELSIUS = 0,
    FARENHEIT = 1
} TEMP_TYPE_e;

typedef struct DEVICE_DS18B20_s {

	struct ow{
		uint8_t port;
		uint8_t pin;
	}OW_s;
    struct ROMADDR {
        uint8_t ROMAddressID[8];
    } ROMADDR_s;

    struct CFGBITS {
        unsigned reserved : 5; 	//reads 1 always
        unsigned R0 : 1;
        unsigned R1 : 1;
        unsigned : 1; 			// reads 0 always
    } CONFIGBits_s;

    struct SPAD {
    	uint8_t TempLow;
    	uint8_t TempHi;
    	uint8_t UserByte1;
    	uint8_t UserByte2;
    	uint8_t Config; 			//config register
    	uint8_t reserved0; 		// unknown
    	uint8_t reserved1; 		// reads 0x10
    	uint8_t reserved2;
    	uint8_t CRC; 			// Verify CRC calculations
    } ScratchPad_s;

    struct TEMP {
    	uint8_t Temp_LowByte;
    	uint8_t Temp_HighByte;
    	uint8_t Temp_Whole;
    	uint8_t Temp_Decimal;
        TEMP_TYPE_e Temp_Type;
        uint8_t Highest_Temp;
        uint8_t Lowest_Temp;
    } Temperature_s;
} DS18b20_t;

//DS18b20_t ds18b20; /* Create an instance object for DS18B20 */

//Define the type of Power Supply that a device has in a particular time.

typedef enum {
    PARASITE,
    EXTERNAL
} POWERTYPE_t;

// Functions Prototypes
Bool DS18B20Init(DS18b20_t* );
Bool DS18B20CmdGetROMIDCode(DS18b20_t*);
Bool DS18B20ConfirmIDofDevice(DS18b20_t*);
Bool DS18B20WriteAlarm_CfgReg(uint16_t Alarm, uint8_t Config);
Bool DS18B20GetTemperature(DS18b20_t*);
void DS18B20RestoreEEPROMValues(void);
void DS18B20SaveSRAMValues(void);
POWERTYPE_t DS18B20GetPWRSupplyStatus(void);
uint8_t DS18B20CalcAndCompareCRC(uint8_t _crc);
Bool DS18B20GetScrachtPadReg(DS18b20_t*);


#ifdef	__cplusplus
}
#endif

#endif	/* T_18B20_H */

#endif
