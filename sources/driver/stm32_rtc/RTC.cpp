#include "RTC.h"
#include "main.h"
#ifdef __cplusplus
extern "C"
{
#endif
const char* 	dayNames[7]   = { "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun" };
const char* 	monthNames[12]   = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};


void RTC_Init(void)
{	 
	//For RTC
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	PWR_BackupAccessCmd(ENABLE);
	RCC_LSEConfig(RCC_LSE_ON);		
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE); 
	RCC_RTCCLKCmd(ENABLE);
	PWR_BackupAccessCmd(DISABLE);
}

static bool isLeapYear(unsigned int year)
{
   // leap year if perfectly divisible by 400
   if (year % 400 == 0) return 1;
	
   // not a leap year if divisible by 100
   // but not divisible by 400
   else if (year % 100 == 0) return 0;

   // leap year if not divisible by 100
   // but divisible by 4
   else if (year % 4 == 0) return 1;
	
   // all other years are not leap years
   else return 0;
}

// This function will return week day number from 0 to 6
static uint8_t returnWeekday(uint16_t year, uint8_t month, uint8_t day)
{
    uint8_t wday = 0;
    wday = (day + ((153 * (month + 12 * ((14 - month) / 12) - 3) + 2) / 5)
               + (365 * (year + 4800 - ((14 - month) / 12)))
               + ((year + 4800 - ((14 - month) / 12)) / 4)
               - ((year + 4800 - ((14 - month) / 12)) / 100)
               + ((year + 4800 - ((14 - month) / 12)) / 400)
               - 32045)	% 7;
    return wday;
}

RTCSTM32::RTCSTM32(){}
RTCSTM32::~RTCSTM32(){}

void RTCSTM32::init(void)
{
	__disable_irq();
	RTC_Init();	
	if(!IS_RTC_RUNNING()) //Init for a first time
	{				
		PWR_BackupAccessCmd(ENABLE);
		
		RTC_WaitForSynchro();
		
		RTC_WaitForLastTask();			//This is quite important to wait before and after any write operation
		RTC_SetPrescaler(32768-1);
		RTC_WaitForLastTask();

		RTC_WaitForLastTask();
		RTC_SetCounter(BASE_SECOND);
		RTC_WaitForLastTask();
		
		BKP->DR1 |= 0x8000; //assign first bit in DR1 in order not to init at the next time		
		PWR_BackupAccessCmd(DISABLE);
	}
	__enable_irq();
}
RTCDateTime RTCSTM32::getDateTime(void)
{
	RTC_WaitForLastTask();
	uint32_t counts  = RTC_GetCounter();
	uint16_t year; uint8_t month; uint32_t day; uint8_t hour; uint8_t minute; uint8_t second;

	day = (counts / 86400);
	counts = counts - day*86400;
	hour = counts / 3600;			//Hour is done
	minute = (counts % 3600) / 60;	//Minute is done
	second = (counts % 3600) % 60;	//Second is done
	year = BASE_YEAR;
	while(day > 365)
	{
		if(isLeapYear(year))
		{
			if(day > 366)
			{
				day -= 366;
			}                 
			else day -= 365;
		}
		else day -= 365;
		year++;			
	}
	year++;		
	//Year is done

	month = BASE_MONTH;
	while(month < 13)
	{
		if(day < day_of_month[month]) break;
		day -= day_of_month[month];
		if(month++ == 2 && isLeapYear(year)) day--;
	}
	//Month is done
	//Day also
	uint8_t dayOfWeek = returnWeekday(year, month, day); //Day of week

	date_time.year = year;
	date_time.month = month;
	date_time.day = day;
	date_time.hour = hour;
	date_time.minute = minute;
	date_time.second = second;
	date_time.dayOfWeek	= dayNames[dayOfWeek];
	date_time.monthOfYear = monthNames[month-1];
	
	return date_time;
}
void RTCSTM32::setDateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
{	
	uint32_t counts = 0;
	uint32_t num_day = 0;
	num_day += day;
	month--;
	while(month > 0)
	{
		num_day += day_of_month[month];
		if(month-- == 2 && isLeapYear(year)) num_day++;
	}
	year--;
	num_day += (year - BASE_YEAR) * 365 + ((uint8_t)((year - BASE_YEAR)/4)) + 1; //+1 Bcause 2020 is leap
	counts += num_day*86400 + hour*3600 + minute*60 + second;
	if((BKP->DR1 & 0x8000) > 0)
	{
		PWR_BackupAccessCmd(ENABLE);
		RTC_WaitForLastTask();
		RTC_SetCounter(counts);
		RTC_WaitForLastTask();
		PWR_BackupAccessCmd(DISABLE);
	}
}	

#ifdef __cplusplus
}
#endif