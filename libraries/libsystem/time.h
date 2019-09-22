#pragma once

#include <libsystem/runtime.h>

#define EPOCH_YEAR 1970

#define SECONDS_PER_MINUTE (60)
#define SECONDS_PER_HOURS (SECONDS_PER_MINUTE * 60)
#define SECONDS_PER_DAY (SECONDS_PER_HOURS * 24)
#define MONTH_PER_YEAR 12


static const int DAYS_PER_MONTH[2][MONTH_PER_YEAR] = {
    {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
    {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
};

static const int DAYS_PER_YEAR[2] = {365, 366};

#define IS_LEAP_YEAR(__year) \
    ((!((__year) % 4) && (__year) % 100) || !((__year) % 400))

typedef u32 elapsedtime_t;

typedef u32 timestamp_t;

typedef struct
{
    int second;
    int minute;
    int hour;
} time_t;

typedef struct
{
    int day;
    int month;
    int year;
} date_t;

typedef union {
    struct
    {
        time_t time;
        date_t date;
    };

    struct
    {
        int second;
        int minute;
        int hour;
        int day;
        int month;
        int year;
    };
} datetime_t;

timestamp_t timestamp_now(void);

time_t timestamp_to_time(timestamp_t timestamp);

date_t timestamp_to_date(timestamp_t timestamp);

datetime_t timestamp_to_datetime(timestamp_t timestamp);

timestamp_t datetime_to_timestamp(datetime_t datetime);

datetime_t datetime_now(void);
