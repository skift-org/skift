#pragma once

#include <libutils/Prelude.h>

typedef uint32_t ElapsedTime;

typedef uint32_t TimeStamp;

typedef uint32_t Timeout;

typedef uint32_t Tick;

struct Time
{
    int second;
    int minute;
    int hour;
};

struct Date
{
    int day;
    int month;
    int year;
};

union DateTime
{
    struct
    {
        Time time;
        Date date;
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
};
