/*
 * DS3231.c
 *
 *  Created on: Dec 15, 2019
 *      Author: dsolano
 */

#include <Elegoo/DS3231.h>
#include <stdio.h>
#include <string.h>

/*
 * PRIVATE FUNCTION AND VARIABLES
 */


static char 	*strDayOfWeek(uint8_t dayOfWeek);
static char 	*strMonth(uint8_t month);
static char 	*strAmPm(uint8_t hour, bool uppercase);
static char 	*strDaySufix(uint8_t day);

static uint8_t 	hour12(uint8_t hour24);
static uint8_t 	bcd2dec(uint8_t bcd);
static uint8_t 	dec2bcd(uint8_t dec);

static long 	time2long(uint16_t days, uint8_t hours, uint8_t minutes, uint8_t seconds);
static uint16_t date2days(uint16_t year, uint8_t month, uint8_t day);
static uint8_t 	daysInMonth(uint16_t year, uint8_t month);
static uint16_t dayInYear(uint16_t year, uint8_t month, uint8_t day);
static bool 	isLeapYear(uint16_t year);
static uint8_t 	dow(uint16_t y, uint8_t m, uint8_t d);

static uint32_t unixtime(RTCDateTime *t);
static uint8_t 	conv2d(const char* p);

static void 	writeRegister8(uint8_t reg, uint8_t value);
static uint8_t 	readRegister8(uint8_t reg);


/*
DS3231.cpp - Class file for the DS3231 Real-Time Clock

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


#include "Elegoo/DS3231.h"

const uint8_t daysArray[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
const uint8_t dowArray[]  = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
I2C_ID_T i2cx;
char buffer[255];


bool DS3231_initialize(RTCDateTime *t, I2C_ID_T i2cid)
{
    Chip_I2C_Init(i2cid);
    Chip_I2C_SetClockRate(i2cid, 400000);
    Chip_I2C_SetMasterEventHandler(i2cid, Chip_I2C_EventHandlerPolling);

    i2cx = i2cid;

    DS3231_setBattery(true, false);

    t->year = 2000;
    t->month = 1;
    t->day = 1;
    t->hour = 0;
    t->minute = 0;
    t->second = 0;
    t->dayOfWeek = 6;
    t->unixtime = 946681200;

    if (Chip_I2C_MasterSend(i2cx, DS3231_ADDRESS, buffer, 1)) return true;
    else return false;
}

Bool DS3231_setDateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
{
	uint8_t buf[10] = {0};
	I2C_XFER_T xfer = {0};

	buf[0] = DS3231_REG_TIME;
	buf[1] = (dec2bcd(second));
    buf[2] = (dec2bcd(minute));
    buf[3] = (dec2bcd(hour));
    buf[4] = (dec2bcd(dow(year, month, day)));
    buf[5] = (dec2bcd(day));
    buf[6] = (dec2bcd(month));
    buf[7] = (dec2bcd(year-2000));

    xfer.slaveAddr = DS3231_ADDRESS;
    xfer.txBuff = buf;
    xfer.txSz = 8;
	Chip_I2C_MasterTransfer(i2cx, &xfer);

	if (!xfer.txSz) return true;
	else return false;
}

void DS3231_setDateTime_word(uint32_t t)
{
    t -= 946681200;

    uint8_t daysPerMonth;

    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;

    second = t % 60;
    t /= 60;

    minute = t % 60;
    t /= 60;

    hour = t % 24;
    uint16_t days = t / 24;
    uint8_t leap;

    for (year = 0; ; ++year)
    {
        leap = year % 4 == 0;
        if (days < 365 + leap)
        {
            break;
        }
        days -= 365 + leap;
    }

    for (month = 1; ; ++month)
    {
        daysPerMonth = daysArray[month - 1];

        if (leap && month == 2)
        {
            ++daysPerMonth;
        }

        if (days < daysPerMonth)
        {
            break;
        }
        days -= daysPerMonth;
    }

    day = days + 1;

    DS3231_setDateTime(year+2000, month, day, hour, minute, second);
}

void DS3231_setDateTime_array(const char* date, const char* time)
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;

    year = conv2d(date+9);

    switch (date[0])
    {
        case 'J': month = (date[1] == 'a' ? 1 : ((date[2] == 'n' ? 6 : 7))); break;
        case 'F': month = 2; break;
        case 'A': month = (date[2] == 'r' ? 4 : 8); break;
        case 'M': month = (date[2] == 'r' ? 3 : 5); break;
        case 'S': month = 9; break;
        case 'O': month = 10; break;
        case 'N': month = 11; break;
        case 'D': month = 12; break;
    }

    day 	= conv2d(date+4);
    hour 	= conv2d(time);
    minute 	= conv2d(time+3);
    second 	= conv2d(time+6);

    DS3231_setDateTime(year+2000, month, day, hour, minute, second);
}

char* DS3231_dateFormat(const char* dateFormat, RTCDateTime* dt)
{

    buffer[0] = 0;

    char helper[11];

    while (*dateFormat != '\0')
    {
        switch (dateFormat[0])
        {
            // Day decoder
            case 'd':
                sprintf(helper, "%02d", dt->day);
                strcat(buffer, (const char *)helper);
                break;
            case 'j':
                sprintf(helper, "%d", dt->day);
                strcat(buffer, (const char *)helper);
                break;
            case 'l':
                strcat(buffer, (const char *)strDayOfWeek(dt->dayOfWeek));
                break;
            case 'D':
                strncat(buffer, strDayOfWeek(dt->dayOfWeek), 3);
                break;
            case 'N':
                sprintf(helper, "%d", dt->dayOfWeek);
                strcat(buffer, (const char *)helper);
                break;
            case 'w':
                sprintf(helper, "%d", (dt->dayOfWeek + 7) % 7);
                strcat(buffer, (const char *)helper);
                break;
            case 'z':
                sprintf(helper, "%d", dayInYear(dt->year, dt->month, dt->day));
                strcat(buffer, (const char *)helper);
                break;
            case 'S':
                strcat(buffer, (const char *)strDaySufix(dt->day));
                break;

            // Month decoder
            case 'm':
                sprintf(helper, "%02d", dt->month);
                strcat(buffer, (const char *)helper);
                break;
            case 'n':
                sprintf(helper, "%d", dt->month);
                strcat(buffer, (const char *)helper);
                break;
            case 'F':
                strcat(buffer, (const char *)strMonth(dt->month));
                break;
            case 'M':
                strncat(buffer, (const char *)strMonth(dt->month), 3);
                break;
            case 't':
                sprintf(helper, "%d", daysInMonth(dt->year, dt->month));
                strcat(buffer, (const char *)helper);
                break;

            // Year decoder
            case 'Y':
                sprintf(helper, "%d", dt->year);
                strcat(buffer, (const char *)helper);
                break;
            case 'y':
            	sprintf(helper, "%02d", dt->year-2000);
                strcat(buffer, (const char *)helper);
                break;
            case 'L':
                sprintf(helper, "%d", isLeapYear(dt->year));
                strcat(buffer, (const char *)helper);
                break;

            // Hour decoder
            case 'H':
                sprintf(helper, "%02d", dt->hour);
                strcat(buffer, (const char *)helper);
                break;
            case 'G':
                sprintf(helper, "%d", dt->hour);
                strcat(buffer, (const char *)helper);
                break;
            case 'h':
                sprintf(helper, "%02d", hour12(dt->hour));
                strcat(buffer, (const char *)helper);
                break;
            case 'g':
                sprintf(helper, "%d", hour12(dt->hour));
                strcat(buffer, (const char *)helper);
                break;
            case 'A':
                strcat(buffer, (const char *)strAmPm(dt->hour, true));
                break;
            case 'a':
                strcat(buffer, (const char *)strAmPm(dt->hour, false));
                break;

            // Minute decoder
            case 'i':
                sprintf(helper, "%02d", dt->minute);
                strcat(buffer, (const char *)helper);
                break;

            // Second decoder
            case 's':
                sprintf(helper, "%02d", dt->second);
                strcat(buffer, (const char *)helper);
                break;

            // Misc decoder
            case 'U':
                sprintf(helper, "%lu", dt->unixtime);
                strcat(buffer, (const char *)helper);
                break;

            default:
                strncat(buffer, dateFormat, 1);
                break;
        }
        dateFormat++;
    }

    return buffer;
}

char* DS3231_Alarm_dateFormat(const char* dateFormat, RTCAlarmTime* dt)
{

    buffer[0] = 0;

    char helper[11];

    while (*dateFormat != '\0')
    {
        switch (dateFormat[0])
        {
            // Day decoder
            case 'd':
                sprintf(helper, "%02d", dt->day);
                strcat(buffer, (const char *)helper);
                break;
            case 'j':
                sprintf(helper, "%d", dt->day);
                strcat(buffer, (const char *)helper);
                break;
            case 'l':
                strcat(buffer, (const char *)strDayOfWeek(dt->day));
                break;
            case 'D':
                strncat(buffer, strDayOfWeek(dt->day), 3);
                break;
            case 'N':
                sprintf(helper, "%d", dt->day);
                strcat(buffer, (const char *)helper);
                break;
            case 'w':
                sprintf(helper, "%d", (dt->day + 7) % 7);
                strcat(buffer, (const char *)helper);
                break;
            case 'S':
                strcat(buffer, (const char *)strDaySufix(dt->day));
                break;

            // Hour decoder
            case 'H':
                sprintf(helper, "%02d", dt->hour);
                strcat(buffer, (const char *)helper);
                break;
            case 'G':
                sprintf(helper, "%d", dt->hour);
                strcat(buffer, (const char *)helper);
                break;
            case 'h':
                sprintf(helper, "%02d", hour12(dt->hour));
                strcat(buffer, (const char *)helper);
                break;
            case 'g':
                sprintf(helper, "%d", hour12(dt->hour));
                strcat(buffer, (const char *)helper);
                break;
            case 'A':
                strcat(buffer, (const char *)strAmPm(dt->hour, true));
                break;
            case 'a':
                strcat(buffer, (const char *)strAmPm(dt->hour, false));
                break;

            // Minute decoder
            case 'i':
                sprintf(helper, "%02d", dt->minute);
                strcat(buffer, (const char *)helper);
                break;

            // Second decoder
            case 's':
                sprintf(helper, "%02d", dt->second);
                strcat(buffer, (const char *)helper);
                break;

            default:
                strncat(buffer, dateFormat, 1);
                break;
        }
        dateFormat++;
    }

    return buffer;
}

int DS3231_getDateTime(RTCDateTime* t)
{
    uint8_t values[7];
    int result = 0;

    result = Chip_I2C_MasterCmdRead(i2cx, DS3231_ADDRESS, DS3231_REG_TIME, values, 7);

    t->second 		= bcd2dec(values[0]);
    t->minute 		= bcd2dec(values[1]);
    t->hour 		= bcd2dec(values[2]);
    t->dayOfWeek 	= bcd2dec(values[3]);
    t->day 			= bcd2dec(values[4]);
    t->month 		= bcd2dec(values[5]);
    t->year 		= bcd2dec(values[6]) +2000;
    t->unixtime 	= unixtime(t);

    return result;
}

uint8_t DS3231_isReady(void)
{
    return true;
}

void DS3231_enableOutput(bool enabled)
{
    uint8_t value;

    value = readRegister8(DS3231_REG_CONTROL);

    value &= 0b11111011;
    value |= (!enabled << 2);

    writeRegister8(DS3231_REG_CONTROL, value);
}

void DS3231_setBattery(bool timeBattery, bool squareBattery)
{
    uint8_t value;

    value = readRegister8(DS3231_REG_CONTROL);

    if (squareBattery)
    {
        value |= 0b01000000;
    } else
    {
        value &= 0b10111111;
    }

    if (timeBattery)
    {
        value &= 0b01111011;
    } else
    {
        value |= 0b10000000;
    }

    writeRegister8(DS3231_REG_CONTROL, value);
}

bool DS3231_isOutput(void)
{
    uint8_t value;

    value = readRegister8(DS3231_REG_CONTROL);

    value &= 0b00000100;
    value >>= 2;

    return !value;
}

void DS3231_setOutput(DS3231_sqw_t mode)
{
    uint8_t value;

    value = readRegister8(DS3231_REG_CONTROL);

    value &= 0b11100111;
    value |= (mode << 3);

    writeRegister8(DS3231_REG_CONTROL, value);
}

DS3231_sqw_t DS3231_getOutput(void)
{
    uint8_t value;

    value = readRegister8(DS3231_REG_CONTROL);

    value &= 0b00011000;
    value >>= 3;

    return (DS3231_sqw_t)value;
}

void DS3231_enable32kHz(bool enabled)
{
    uint8_t value;

    value = readRegister8(DS3231_REG_STATUS);

    value &= 0b11110111;
    value |= (enabled << 3);

    writeRegister8(DS3231_REG_STATUS, value);
}

bool DS3231_is32kHz(void)
{
    uint8_t value;

    value = readRegister8(DS3231_REG_STATUS);

    value &= 0b00001000;
    value >>= 3;

    return value;
}

void DS3231_forceConversion(void)
{
    uint8_t value;

    value = readRegister8(DS3231_REG_CONTROL);

    value |= 0b00100000;

    writeRegister8(DS3231_REG_CONTROL, value);

    do {} while ((readRegister8(DS3231_REG_CONTROL) & 0b00100000) != 0);
}

float DS3231_readTemperature(void)
{
    uint8_t bytes[2] = {0};

    Chip_I2C_MasterCmdRead(i2cx, DS3231_ADDRESS, DS3231_REG_TEMPERATURE, bytes, 2);

    return ((((short)bytes[0] << 8) | (short)bytes[1]) >> 6) / 4.0f;
}

void DS3231_getAlarm1(RTCAlarmTime* a)
{
    uint8_t values[4];

    Chip_I2C_MasterCmdRead(i2cx, DS3231_ADDRESS, DS3231_REG_ALARM_1, values, 4);

    a->day 		= bcd2dec(values[0] & 0b01111111);
    a->hour 	= bcd2dec(values[1] & 0b01111111);
    a->minute 	= bcd2dec(values[2] & 0b01111111);
    a->second 	= bcd2dec(values[3] & 0b01111111);

}

DS3231_alarm1_t DS3231_getAlarmType1(void)
{
    uint8_t values[4];
    uint8_t mode = 0;

    Chip_I2C_MasterCmdRead(i2cx, DS3231_ADDRESS, DS3231_REG_ALARM_1, values, 4);

    mode |= bcd2dec((values[3] & 0b01000000) >> 6);
    mode |= bcd2dec((values[2] & 0b01000000) >> 5);
    mode |= bcd2dec((values[1] & 0b01000000) >> 4);
    mode |= bcd2dec((values[0] & 0b01000000) >> 3);
    mode |= bcd2dec((values[0] & 0b00100000) >> 1);

    return (DS3231_alarm1_t)mode;
}

void DS3231_setAlarm1(uint8_t dydw, uint8_t hour, uint8_t minute, uint8_t second, DS3231_alarm1_t mode, bool armed)
{
	uint8_t buff[8] = {0};

    second 	= dec2bcd(second);
    minute 	= dec2bcd(minute);
    hour 	= dec2bcd(hour);
    dydw 	= dec2bcd(dydw);

    switch(mode)
    {
        case DS3231_EVERY_SECOND:
            second 	|= 0b10000000;
            minute 	|= 0b10000000;
            hour 	|= 0b10000000;
            dydw 	|= 0b10000000;
            break;

        case DS3231_MATCH_S:
            second 	&= 0b01111111;
            minute 	|= 0b10000000;
            hour 	|= 0b10000000;
            dydw 	|= 0b10000000;
            break;

        case DS3231_MATCH_M_S:
            second 	&= 0b01111111;
            minute 	&= 0b01111111;
            hour 	|= 0b10000000;
            dydw 	|= 0b10000000;
            break;

        case DS3231_MATCH_H_M_S:
            second 	&= 0b01111111;
            minute 	&= 0b01111111;
            hour 	&= 0b01111111;
            dydw 	|= 0b10000000;
            break;

        case DS3231_MATCH_DT_H_M_S:
            second 	&= 0b01111111;
            minute 	&= 0b01111111;
            hour 	&= 0b01111111;
            dydw 	&= 0b01111111;
            break;

        case DS3231_MATCH_DY_H_M_S:
            second 	&= 0b01111111;
            minute 	&= 0b01111111;
            hour 	&= 0b01111111;
            dydw 	&= 0b01111111;
            dydw 	|= 0b01000000;
            break;
    }

    buff[0] = DS3231_REG_ALARM_1;
    buff[1] = second;
    buff[2] = minute;
    buff[3] = hour;
    buff[4] = dydw;

    Chip_I2C_MasterSend(i2cx, DS3231_ADDRESS, buff, 5);

    DS3231_armAlarm1(armed);

    DS3231_clearAlarm1();
}

bool DS3231_isAlarm1(bool clear)
{
    uint8_t alarm;

    alarm = readRegister8(DS3231_REG_STATUS);
    alarm &= 0b00000001;

    if (alarm && clear)
    {
    	DS3231_clearAlarm1();
    }

    return alarm;
}

void DS3231_armAlarm1(bool armed)
{
    uint8_t value;
    value = readRegister8(DS3231_REG_CONTROL);

    if (armed)
    {
        value |= 0b00000001;
    } else
    {
        value &= 0b11111110;
    }

    writeRegister8(DS3231_REG_CONTROL, value);
}

bool DS3231_isArmed1(void)
{
    uint8_t value;
    value = readRegister8(DS3231_REG_CONTROL);
    value &= 0b00000001;
    return value;
}

void DS3231_clearAlarm1(void)
{
    uint8_t value;

    value = readRegister8(DS3231_REG_STATUS);
    value &= 0b11111110;

    writeRegister8(DS3231_REG_STATUS, value);
}

void DS3231_getAlarm2(RTCAlarmTime* a)
{
    uint8_t values[4];

    Chip_I2C_MasterCmdRead(i2cx, DS3231_ADDRESS, DS3231_REG_ALARM_2, values, 4);

    a->day 		= bcd2dec(values[0] & 0b01111111);
    a->hour 	= bcd2dec(values[1] & 0b01111111);
    a->minute 	= bcd2dec(values[2] & 0b01111111);
    a->second 	= bcd2dec(values[3] & 0b01111111);
}

DS3231_alarm2_t DS3231_getAlarmType2(void)
{
    uint8_t values[4];
    uint8_t mode = 0;

    Chip_I2C_MasterCmdRead(i2cx, DS3231_ADDRESS, DS3231_REG_ALARM_2, values, 4);

    mode |= bcd2dec((values[3] & 0b01000000) >> 6);
    mode |= bcd2dec((values[2] & 0b01000000) >> 5);
    mode |= bcd2dec((values[1] & 0b01000000) >> 4);
    mode |= bcd2dec((values[0] & 0b01000000) >> 3);
    mode |= bcd2dec((values[0] & 0b00100000) >> 1);

    return (DS3231_alarm2_t)mode;
}

void DS3231_setAlarm2(uint8_t dydw, uint8_t hour, uint8_t minute, DS3231_alarm2_t mode, bool armed)
{
	uint8_t buff[8] = {0};

    minute 	= dec2bcd(minute);
    hour 	= dec2bcd(hour);
    dydw 	= dec2bcd(dydw);

    switch(mode)
    {
        case DS3231_EVERY_MINUTE:
            minute 	|= 0b10000000;
            hour 	|= 0b10000000;
            dydw 	|= 0b10000000;
            break;

        case DS3231_MATCH_M:
            minute 	&= 0b01111111;
            hour 	|= 0b10000000;
            dydw 	|= 0b10000000;
            break;

        case DS3231_MATCH_H_M:
            minute 	&= 0b01111111;
            hour 	&= 0b01111111;
            dydw 	|= 0b10000000;
            break;

        case DS3231_MATCH_DT_H_M:
            minute 	&= 0b01111111;
            hour 	&= 0b01111111;
            dydw 	&= 0b01111111;
            break;

        case DS3231_MATCH_DY_H_M:
            minute 	&= 0b01111111;
            hour 	&= 0b01111111;
            dydw 	&= 0b01111111;
            dydw 	|= 0b01000000;
            break;
    }

    buff[0] = DS3231_REG_ALARM_2;
    buff[1] = minute;
    buff[2] = hour;
    buff[3] = dydw;

    Chip_I2C_MasterSend(i2cx, DS3231_ADDRESS, buff, 4);

    DS3231_armAlarm2(armed);

    DS3231_clearAlarm2();
}

void DS3231_armAlarm2(bool armed)
{
    uint8_t value;
    value = readRegister8(DS3231_REG_CONTROL);

    if (armed)
    {
        value |= 0b00000010;
    } else
    {
        value &= 0b11111101;
    }

    writeRegister8(DS3231_REG_CONTROL, value);
}

bool DS3231_isArmed2(void)
{
    uint8_t value;
    value = readRegister8(DS3231_REG_CONTROL);
    value &= 0b00000010;
    value >>= 1;
    return value;
}


void DS3231_clearAlarm2(void)
{
    uint8_t value;

    value = readRegister8(DS3231_REG_STATUS);
    value &= 0b11111101;

    writeRegister8(DS3231_REG_STATUS, value);
}


bool DS3231_isAlarm2(bool clear)
{
    uint8_t alarm;

    alarm = readRegister8(DS3231_REG_STATUS);
    alarm &= 0b00000010;

    if (alarm && clear)
    {
    	DS3231_clearAlarm2();
    }

    return alarm;
}

static uint8_t bcd2dec(uint8_t bcd)
{
    return ((bcd / 16) * 10) + (bcd % 16);
}

static uint8_t dec2bcd(uint8_t dec)
{
    return ((dec / 10) * 16) + (dec % 10);
}

static char *strDayOfWeek(uint8_t dayOfWeek)
{
    switch (dayOfWeek) {
        case 1:
            return "Monday";
            break;
        case 2:
            return "Tuesday";
            break;
        case 3:
            return "Wednesday";
            break;
        case 4:
            return "Thursday";
            break;
        case 5:
            return "Friday";
            break;
        case 6:
            return "Saturday";
            break;
        case 7:
            return "Sunday";
            break;
        default:
            return "Unknown";
    }
}

static char *strMonth(uint8_t month)
{
    switch (month) {
        case 1:
            return "January";
            break;
        case 2:
            return "February";
            break;
        case 3:
            return "March";
            break;
        case 4:
            return "April";
            break;
        case 5:
            return "May";
            break;
        case 6:
            return "June";
            break;
        case 7:
            return "July";
            break;
        case 8:
            return "August";
            break;
        case 9:
            return "September";
            break;
        case 10:
            return "October";
            break;
        case 11:
            return "November";
            break;
        case 12:
            return "December";
            break;
        default:
            return "Unknown";
    }
}

static char *strAmPm(uint8_t hour, bool uppercase)
{
    if (hour < 12)
    {
        if (uppercase)
        {
            return "AM";
        } else
        {
            return "am";
        }
    } else
    {
        if (uppercase)
        {
            return "PM";
        } else
        {
            return "pm";
        }
    }
}

static char *strDaySufix(uint8_t day)
{
    if (day % 10 == 1)
    {
        return "st";
    } else
    if (day % 10 == 2)
    {
        return "nd";
    }
    if (day % 10 == 3)
    {
        return "rd";
    }

    return "th";
}

static uint8_t hour12(uint8_t hour24)
{
    if (hour24 == 0)
    {
        return 12;
    }

    if (hour24 > 12)
    {
       return (hour24 - 12);
    }

    return hour24;
}

static long time2long(uint16_t days, uint8_t hours, uint8_t minutes, uint8_t seconds)
{
    return ((days * 24L + hours) * 60 + minutes) * 60 + seconds;
}

static uint16_t dayInYear(uint16_t year, uint8_t month, uint8_t day)
{
    uint16_t fromDate;
    uint16_t toDate;

    fromDate = date2days(year, 1, 1);
    toDate = date2days(year, month, day);

    return (toDate - fromDate);
}

static bool isLeapYear(uint16_t year)
{
    return (year % 4 == 0);
}

static uint8_t daysInMonth(uint16_t year, uint8_t month)
{
    uint8_t days;

    days = *(daysArray + month - 1);

    if ((month == 2) && isLeapYear(year))
    {
        ++days;
    }

    return days;
}

static uint16_t date2days(uint16_t year, uint8_t month, uint8_t day)
{
    year = year - 2000;

    uint16_t days16 = day;

    for (uint8_t i = 1; i < month; ++i)
    {
        days16 += *(daysArray + i - 1);
    }

    if ((month == 2) && isLeapYear(year))
    {
        ++days16;
    }

    return days16 + 365 * year + (year + 3) / 4 - 1;
}

static uint32_t unixtime(RTCDateTime *t)
{
    uint32_t u;

    u = time2long(date2days(t->year, t->month, t->day), t->hour, t->minute, t->second);
    u += 946681200;

    return u;
}

static uint8_t conv2d(const char* p)
{
    uint8_t v = 0;

    if ('0' <= *p && *p <= '9')
    {
        v = *p - '0';
    }

    return 10 * v + *++p - '0';
}

static uint8_t dow(uint16_t y, uint8_t m, uint8_t d)
{
    uint8_t dow;

    y -= m < 3;
    dow = ((y + y/4 - y/100 + y/400 + *(dowArray+(m-1)) + d) % 7);

    if (dow == 0)
    {
        return 7;
    }

    return dow;
}

static void writeRegister8(uint8_t reg, uint8_t value)
{
	uint8_t buff[2] = {0};
	buff[0] = reg;
	buff[1] = value;
	Chip_I2C_MasterSend(i2cx, DS3231_ADDRESS, buff, 2);
}

static uint8_t readRegister8(uint8_t reg)
{
    uint8_t value;

	Chip_I2C_MasterCmdRead(i2cx, DS3231_ADDRESS, reg, &value, 1);

    return value;
}
