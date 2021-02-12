#include <assert.h>
#include <time.h>

#include <skift/Time.h>

time_t time(time_t *timer)
{
    time_t now = (time_t)timestamp_now();
    *timer = now;
    return now;
}

clock_t clock(void)
{
    return 1;
}

struct tm *gmtime(const time_t *timer)
{
    DateTime datetime = timestamp_to_datetime((TimeStamp)*timer);

    static struct tm tmstruct;

    tmstruct.tm_sec = datetime.second;
    tmstruct.tm_min = datetime.minute;
    tmstruct.tm_hour = datetime.hour;
    tmstruct.tm_mday = datetime.day;
    tmstruct.tm_mon = datetime.month;
    tmstruct.tm_year = datetime.year;
    tmstruct.tm_wday = 0;
    tmstruct.tm_yday = 0;
    tmstruct.tm_isdst = -1;

    return &tmstruct;
}

struct tm *localtime(const time_t *timer)
{
    return gmtime(timer);
}

size_t strftime(char *s, size_t n, const char *format, const struct tm *tptr)
{
    __unused(s);
    __unused(n);
    __unused(format);
    __unused(tptr);

    return 0;
}

time_t mktime(struct tm *ptr)
{
    __unused(ptr);
    ASSERT_NOT_REACHED();
}