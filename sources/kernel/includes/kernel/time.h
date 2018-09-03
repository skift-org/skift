#pragma once

/* Copyright © 2018 MAKER.                                                    */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "types.h"

#define TIME_SECOND 0x00
#define TIME_MINUTE 0x02
#define TIME_HOUR 0x04
#define TIME_DAY 0x07
#define TIME_MONTH 0x08
#define TIME_YEAR 0x09

typedef struct
{
    u8 second;
    u8 minute;
    u8 hour;
    u8 day;
    u8 month;
    u32 year;
} time_t;

time_t time();
u32 time_get(u32 selector);
void* time_task(void* arg);