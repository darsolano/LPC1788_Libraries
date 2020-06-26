/*
 * datetime.h
 *
 *  Created on: 16/12/2014
 *      Author: dsolano
 */

#ifndef DATETIME_H_
#define DATETIME_H_

#include <chip.h>
#include <lpc_types.h>

#define DATE_TIME_CLOCK		LPC_RTC

typedef enum
{
	_12HOUR = 0,
	_24HOUR
}TIME_TYPE_e;


typedef enum
{
	HOUR_AM = 0,
	HOUR_PM
}HOUR_TYPE_e;

typedef  struct
{
	uint8_t SEC; 		/*!< Seconds Register */
	uint8_t MIN; 		/*!< Minutes Register */
	uint8_t HOUR; 		/*!< Hours Register */
	TIME_TYPE_e TIME_TYPE;	// 24 hour type or 12 hour type
	HOUR_TYPE_e HOUR_TYPE;	// AM hour or PM hour
	uint8_t* sAMPM;
} __attribute__((__packed__)) RTCTIME_s;

typedef  struct
{
	uint8_t *sDOW;		// Day of week string
	uint8_t *sMONTH;	// String for month
	uint8_t DOM;		/*!< Day of Month Register */
	uint8_t DOW; 		/*!< Day of Week Register */
	uint8_t MONTH; 		/*!< Months Register */
	uint16_t DOY; 		/*!< Day of Year Register */
	uint16_t YEAR; 		/*!< Years Register, must set it with century included eg:2016 */
} __attribute__((__packed__)) RTCDATE_s;



void StartClockTimer (RTCTIME_s* ptime, RTCDATE_s* pdate);
void GetClock_Time(RTCTIME_s *ptime);
void GetClock_Date(RTCDATE_s *pdate);
void SetClock_Time(RTCTIME_s *ptime);
void SetCLock_Date(RTCDATE_s *pdate);
void SetCLock_AlarmSet(RTCTIME_s *alarm);
#endif /* DATETIME_H_ */
