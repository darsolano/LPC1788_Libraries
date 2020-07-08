/*
 * datetime.h
 *
 *  Created on: 16/12/2014
 *      Author: dsolano
 */

#ifndef RTCCALENDAR_H_
#define RTCCALENDAR_H_

/*
DS3231.h - Header file for the DS3231 Real-Time Clock

Version: 1.0.1
(c) 2014 Korneliusz Jarzebski
www.jarzebski.pl

This program is free software: you can redistribute it and/or modify
it under the terms of the version 3 GNU General Public License as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <chip.h>
#include <lpc_types.h>
#include <stdint.h>


typedef struct
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t dayOfWeek;
    long 	unixtime;
}LPCRTCDateTime;




bool lpcrtc_initialize(LPCRTCDateTime *t);

Bool lpcrtc_setDateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
void lpcrtc_setDateTime_word(uint32_t t);
void lpcrtc_setDateTime_array(const char* date, const char* time);
int  lpcrtc_getDateTime(LPCRTCDateTime*);

char* lpcrtc_dateFormat(const char* dateFormat, LPCRTCDateTime* dt);
//char* DS3231_Alarm_dateFormat(const char* dateFormat, RTCAlarmTime* dt);

#endif /* RTCCALENDAR_H_ */
