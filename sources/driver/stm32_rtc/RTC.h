#ifndef __RTC_H__
#define __RTC_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
	
	
/** Min year 2020
 *  Max year 2136
 *  This can be more but those years are enough
*/

#define BASE_YEAR 2020
#define BASE_MONTH 1
#define BASE_DAY 1
#define BASE_HOUR 0
#define BASE_MINUTE 0
#define BASE_SECOND 0

const uint8_t day_of_month[13] = {0, 31, 28/*Special*/, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

struct RTCDateTime
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    const char* dayOfWeek;
	const char* monthOfYear;
};

struct RTCAlarmTime
{
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
};

#define IS_RTC_RUNNING() ((BKP->DR1 & 0x8000) > 0)

extern RTCDateTime myTime;

class RTCSTM32
{
	private:
		RTCDateTime date_time;
	public:
		RTCSTM32();
		~RTCSTM32();
		void init(void);
		RTCDateTime getDateTime(void);
		void setDateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
};



#ifdef __cplusplus
}
#endif
#endif