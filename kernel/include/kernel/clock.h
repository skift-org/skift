#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/runtime.h>

typedef enum
{
    T_SECOND = 0x0,
    T_MINUTE = 0x2,
    T_HOUR   = 0x4,
    T_DAY    = 0x7,
    T_MONTH  = 0x8,
    T_YEAR   = 0x9,
} time_selector_t;

typedef struct
{
    uint second;
    uint minute;
    uint hour;
    uint day;
    uint month;
    uint year;
} time_t;

void clock_time(time_t *time);
uint clock_read(time_selector_t selector);