/*
 * own_config.h
 *
 *  Created on: Dec 7, 2018
 *      Author: dsolano
 *      copy this file into the project and select the libraries to include
 *      in your application from the current OWNLIB library
 *
 */

#ifndef OWN_CONFIG_H_
#define OWN_CONFIG_H_

#define OWNLIB_CLIENT_TERMINAL_MENU		0	// * Client terminal menu
#define OWNLIB_FLASH_FILE_SYSTEM_LOG	0	// * Flash log File System Manager
#define OWNLIB_FAT_FILE_SYSTEM			0	// * fat file system interface for MCU
#define OWNLIB_OLED_DISPLAY				0	// * Small OLED display Driver
#define OWNLIB_ESP8266_WIFI_AT_DRIVER	0	// * ESP8266 AT driver interface
#define OWNLIB_ONEWIRE_DRIVER			0	// * 1 Wire driver libraries
#define OWNLIB_ADXL335_ACCELEROMETER	0	// * ADXL335 accelerometer driver
#define OWNLIB_BTHC06_BLUETOOTH			0	// * Bluetooth interface driver BTHC06
#define OWNLIB_RTC_INTERFACE			0	// * Easy interface for CPU RTC LPC_17xx
#define OWNLIB_FLASH_FILE_SYSTEM_LOG	0	// * Flash log File System Manager
#define OWNLIB_FLASH_FILE_SYSTEM_LOG	0	// * Flash log File System Manager
#define OWNLIB_FLASH_FILE_SYSTEM_LOG	0	// * Flash log File System Manager
#define OWNLIB_FLASH_FILE_SYSTEM_LOG	0	// * Flash log File System Manager
#define OWNLIB_FLASH_FILE_SYSTEM_LOG	0	// * Flash log File System Manager
#define OWNLIB_FLASH_FILE_SYSTEM_LOG	0	// * Flash log File System Manager
#define OWNLIB_FLASH_FILE_SYSTEM_LOG	0	// * Flash log File System Manager



/*
 * Client terminal menu
 */
#if OWNLIB_CLIENT_TERMINAL_MENU
#include <Cli/cli.h>
#endif

/*
 * Flash log File System Manager
 */
#if FLASH_FILE_SYSTEM_LOG
#include <data_Manager/at45d.h>
#include <data_Manager/log_fs.h>
#endif


#endif /* OWN_CONFIG_H_ */
