/*--------------------------------------------------------------------------*/
/*  RTC controls                                                            */

#include <avr/io.h>
#include <string.h>
#include "rtc.h"


int rtc_gettime (RTCLK *rtc)
{
	rtc->sec = 30;
	rtc->min = 59;
	rtc->hour = 11;
	rtc->mday = 3;
	rtc->month = 9;
	rtc->year = 2009;

	return 1;
}




int rtc_settime (const RTCLK *rtc)
{

	BYTE buf[8];

	buf[0] = rtc->sec / 10 * 16 + rtc->sec % 10;
	buf[1] = rtc->min / 10 * 16 + rtc->min % 10;
	buf[2] = rtc->hour / 10 * 16 + rtc->hour % 10;
	buf[3] = rtc->wday & 7;
	buf[4] = rtc->mday / 10 * 16 + rtc->mday % 10;
	buf[5] = rtc->month / 10 * 16 + rtc->month % 10;
	buf[6] = (rtc->year - 2000) / 10 * 16 + (rtc->year - 2000) % 10;

	return 1;
}




int rtc_init (void)
{
	return 1;
}

