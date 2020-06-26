/*
 * DS3231.h
 *
 *  Created on: Dec 15, 2019
 *      Author: dsolano
 */

#ifndef INC_ELEGOO_DS3231_H_
#define INC_ELEGOO_DS3231_H_

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


#define DS3231_ADDRESS              (0x68)

#define DS3231_REG_TIME             (0x00)
#define DS3231_REG_ALARM_1          (0x07)
#define DS3231_REG_ALARM_2          (0x0B)
#define DS3231_REG_CONTROL          (0x0E)
#define DS3231_REG_STATUS           (0x0F)
#define DS3231_REG_TEMPERATURE      (0x11)

typedef struct
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t dayOfWeek;
    uint32_t unixtime;
}RTCDateTime;

typedef struct
{
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
}RTCAlarmTime;

typedef enum
{
    DS3231_1HZ          = 0x00,
    DS3231_4096HZ       = 0x01,
    DS3231_8192HZ       = 0x02,
    DS3231_32768HZ      = 0x03
} DS3231_sqw_t;

typedef enum
{
    DS3231_EVERY_SECOND   = 0b00001111,
    DS3231_MATCH_S        = 0b00001110,
    DS3231_MATCH_M_S      = 0b00001100,
    DS3231_MATCH_H_M_S    = 0b00001000,
    DS3231_MATCH_DT_H_M_S = 0b00000000,
    DS3231_MATCH_DY_H_M_S = 0b00010000
} DS3231_alarm1_t;

typedef enum
{
    DS3231_EVERY_MINUTE   = 0b00001110,
    DS3231_MATCH_M        = 0b00001100,
    DS3231_MATCH_H_M      = 0b00001000,
    DS3231_MATCH_DT_H_M   = 0b00000000,
    DS3231_MATCH_DY_H_M   = 0b00010000
} DS3231_alarm2_t;



bool DS3231_initialize(RTCDateTime *t, I2C_ID_T i2cid);

Bool DS3231_setDateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
void DS3231_setDateTime_word(uint32_t t);
void DS3231_setDateTime_array(const char* date, const char* time);
int  DS3231_getDateTime(RTCDateTime*);
uint8_t DS3231_isReady(void);

DS3231_sqw_t DS3231_getOutput(void);
void DS3231_setOutput(DS3231_sqw_t mode);
void DS3231_enableOutput(bool enabled);
bool DS3231_isOutput(void);
void DS3231_enable32kHz(bool enabled);
bool DS3231_is32kHz(void);

void DS3231_forceConversion(void);
float DS3231_readTemperature(void);

void DS3231_setAlarm1(uint8_t dydw, uint8_t hour, uint8_t minute, uint8_t second, DS3231_alarm1_t mode, bool armed);
void DS3231_getAlarm1(RTCAlarmTime*);
DS3231_alarm1_t DS3231_getAlarmType1(void);
bool DS3231_isAlarm1(bool clear);
void DS3231_armAlarm1(bool armed);
bool DS3231_isArmed1(void);
void DS3231_clearAlarm1(void);

void DS3231_setAlarm2(uint8_t dydw, uint8_t hour, uint8_t minute, DS3231_alarm2_t mode, bool armed);
void DS3231_getAlarm2(RTCAlarmTime*);
DS3231_alarm2_t DS3231_getAlarmType2(void);
bool DS3231_isAlarm2(bool clear);
void DS3231_armAlarm2(bool armed);
bool DS3231_isArmed2(void);
void DS3231_clearAlarm2(void);

void DS3231_setBattery(bool timeBattery, bool squareBattery);

char* DS3231_dateFormat(const char* dateFormat, RTCDateTime* dt);
char* DS3231_Alarm_dateFormat(const char* dateFormat, RTCAlarmTime* dt);




#endif /* INC_ELEGOO_DS3231_H_ */
