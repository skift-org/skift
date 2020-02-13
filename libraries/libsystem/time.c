/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/__plugs__.h>
#include <libsystem/time.h>

TimeStamp timestamp_now(void)
{
    return __plug_system_get_time();
}

Time timestamp_to_time(TimeStamp timestamp)
{
    return (Time){
        .second = timestamp % 60,
        .minute = (timestamp / SECONDS_PER_MINUTE) % 60,
        .hour = (timestamp / SECONDS_PER_HOURS) % 24,
    };
}

Date timestamp_to_date(TimeStamp timestamp)
{
    Date date = {0};

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

DateTime timestamp_to_datetime(TimeStamp timestamp)
{
    return (DateTime){
        .time = timestamp_to_time(timestamp),
        .date = timestamp_to_date(timestamp),
    };
}

TimeStamp datetime_to_timestamp(DateTime datetime)
{
    TimeStamp timestamp = 0;

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

DateTime datetime_now(void)
{
    return timestamp_to_datetime(timestamp_now());
}
