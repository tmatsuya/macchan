#include <time.h>
#include <sys/time.h>

time_t *getfaketime()
{
	struct tm t;

	t.tm_year = 2011-1900;	/* YEAR-1900年 */
	t.tm_mon  = 10-1;	/* Month-1 */
	t.tm_mday = 9;		/* Day  */
	t.tm_wday = 0;		/* Sunday */
	t.tm_hour = 23;		/* Hour  */
	t.tm_min  = 59;		/* Min  */
	t.tm_sec  = 0;		/* Sec  */
	t.tm_isdst= -1;		/* No summertime */

	return (time_t *)mktime(&t);
}

int clock_gettime(clockid_t clk_id, struct timespec *tp)
{
	tp->tv_sec = (__time_t)getfaketime();

	return 0;
}

int gettimeofday(struct timeval *tv, struct timezone *tz)
{
	tv->tv_sec = (__time_t)getfaketime();

	return 0;
}
