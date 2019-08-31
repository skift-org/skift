#include <libsystem/__plugs__.h>
#include <libsystem/time.h>

timestamp_t timestamp_now(void)
{
    return __plug_system_get_time();
}

time_t timestamp_to_time(timestamp_t timestamp)
{
    return (time_t){
        .second = timestamp % 60,
        .minute = (timestamp / SECONDS_PER_MINUTE) % 60,
        .hour = (timestamp / SECONDS_PER_HOURS) % 24,
    };
}

date_t timestamp_to_date(timestamp_t timestamp)
{
    date_t date = {0};

    int days = timestamp / SECONDS_PER_DAY;

    date.year = EPOCH_YEAR;
    while (days - DAYS_PER_YEAR[IS_LEAP_YEAR(date.year)] > 0)
    {
        date.year++;
        days -= DAYS_PER_YEAR[IS_LEAP_YEAR(date.year)];
    }

    date.month = 0;
    while (days - DAYS_PER_MONTH[IS_LEAP_YEAR(date.year)][date.month] > 0)
    {
        date.month++;
        days -= DAYS_PER_MONTH[IS_LEAP_YEAR(date.year)][date.month];
    }

    date.month++;

    date.day = days + 1;

    return date;
}

datetime_t timestamp_to_datetime(timestamp_t timestamp)
{
    return (datetime_t){
        .time = timestamp_to_time(timestamp),
        .date = timestamp_to_date(timestamp),
    };
}

timestamp_t datetime_to_timestamp(datetime_t datetime)
{
    timestamp_t timestamp = 0;

    for (int year = EPOCH_YEAR; year < datetime.year; year++)
    {
        timestamp += DAYS_PER_YEAR[IS_LEAP_YEAR(year)] * SECONDS_PER_DAY;
    }

    for (int month = 0; month < datetime.month - 1; month++)
    {
        timestamp += DAYS_PER_MONTH[IS_LEAP_YEAR(datetime.year)][month] * SECONDS_PER_DAY;
    }

    timestamp += (datetime.day - 1) * SECONDS_PER_DAY;

    timestamp += datetime.hour * SECONDS_PER_HOURS;
    timestamp += datetime.minute * SECONDS_PER_MINUTE;
    timestamp += datetime.second;

    return timestamp;
}

datetime_t datetime_now(void)
{
    return timestamp_to_datetime(timestamp_now());
}
