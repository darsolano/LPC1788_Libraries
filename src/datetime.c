/*
 * datetime.c
 *
 *  Created on: 16/12/2014
 *      Author: dsolano
 */

#include <datetime.h>


const char *Month[12] = {"Jan\0",
						 "Feb\0",
						 "Mar\0",
						 "Apr\0",
						 "May\0",
						 "Jun\0",
						 "Jul\0",
						 "Aug\0",
						 "Sep\0",
						 "Oct\0",
						 "Nov\0",
						 "Dec\0"};

const char *DayOfWeek[7] = {"Sun\0",
							"Mon\0",
							"Tue\0",
							"Wed\0",
							"Thu\0",
							"Fri\0",
							"Sat\0"};

const char *ampm[2] = {"AM\0", "PM\0"};

const char MonthDay_Relation[12] ={31,28,31,30,31,30,31,31,30,31,30,31};


void StartClockTimer (RTCTIME_s* ptime, RTCDATE_s* pdate)
{
	Chip_RTC_Init(DATE_TIME_CLOCK);
	Chip_RTC_Enable(DATE_TIME_CLOCK, ENABLE);
	Chip_RTC_CalibCounterCmd(DATE_TIME_CLOCK, ENABLE);

	SetClock_Time(ptime);

	SetCLock_Date(pdate);

}

void GetClock_Time(RTCTIME_s *ptime)
{
	uint32_t hour;
	ptime->MIN = Chip_RTC_GetTime(DATE_TIME_CLOCK,RTC_TIMETYPE_MINUTE);
	ptime->SEC = Chip_RTC_GetTime(DATE_TIME_CLOCK, RTC_TIMETYPE_SECOND);
	hour = Chip_RTC_GetTime(DATE_TIME_CLOCK , RTC_TIMETYPE_HOUR);

	if (ptime->TIME_TYPE == _24HOUR)
	{
		ptime->HOUR = Chip_RTC_GetTime(DATE_TIME_CLOCK , RTC_TIMETYPE_HOUR);
	}else if ( ptime->TIME_TYPE == _12HOUR)
	{
		if (hour > 12){
			hour -= 12;
			ptime->HOUR_TYPE = HOUR_PM;
			ptime->sAMPM = (uint8_t*)ampm[HOUR_PM];
			ptime->HOUR = hour;
		}
		else if (hour <= 12){
			if (hour == 0) hour = 12;
			ptime->HOUR_TYPE = HOUR_AM;
			ptime->sAMPM = (uint8_t*) ampm[HOUR_AM];
			ptime->HOUR = hour;
		}
	}
}

void GetClock_Date(RTCDATE_s *pdate)
{
	pdate->DOM = Chip_RTC_GetTime(DATE_TIME_CLOCK,RTC_TIMETYPE_DAYOFMONTH);
	pdate->MONTH = Chip_RTC_GetTime(DATE_TIME_CLOCK,RTC_TIMETYPE_MONTH);
	pdate->YEAR = Chip_RTC_GetTime(DATE_TIME_CLOCK,RTC_TIMETYPE_YEAR);
	pdate->DOW = Chip_RTC_GetTime(DATE_TIME_CLOCK,RTC_TIMETYPE_DAYOFWEEK);
	pdate->sDOW =  (uint8_t*) DayOfWeek[pdate->DOW-1];
	pdate->sMONTH =  (uint8_t*) Month[pdate->MONTH - 1];
}

void SetClock_Time(RTCTIME_s *ptime)
{
	Chip_RTC_SetTime(DATE_TIME_CLOCK, RTC_TIMETYPE_HOUR, ptime->HOUR);
	Chip_RTC_SetTime(DATE_TIME_CLOCK, RTC_TIMETYPE_MINUTE, ptime->MIN);
	Chip_RTC_SetTime(DATE_TIME_CLOCK, RTC_TIMETYPE_SECOND, ptime->SEC);
}


void SetCLock_Date(RTCDATE_s *pdate)
{
	Chip_RTC_SetTime(DATE_TIME_CLOCK, RTC_TIMETYPE_YEAR, pdate->YEAR);
	Chip_RTC_SetTime(DATE_TIME_CLOCK, RTC_TIMETYPE_MONTH, pdate->MONTH);
	Chip_RTC_SetTime(DATE_TIME_CLOCK, RTC_TIMETYPE_DAYOFMONTH, pdate->DOM);
	Chip_RTC_SetTime(DATE_TIME_CLOCK, RTC_TIMETYPE_DAYOFWEEK, pdate->DOW);
}

void SetCLock_AlarmSet(RTCTIME_s *alarm)
{
	Chip_RTC_SetAlarmTime(DATE_TIME_CLOCK, RTC_TIMETYPE_HOUR, alarm->HOUR);
	Chip_RTC_SetAlarmTime(DATE_TIME_CLOCK, RTC_TIMETYPE_MINUTE, alarm->MIN);
}

