/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "kernel/processor.h"

#include "kernel/clock.h"

/* --- CMOS/RTC ------------------------------------------------------------- */

#define CMOS_ADDRESS 0x70
#define CMOS_DATA 0x71

#define from_bcd(val) (((val) / 16) * 10 + ((val)&0xf))

#define CMOS_WAIT while (is_cmos_update())

bool is_cmos_update()
{
    out8(CMOS_ADDRESS, 0x0A);
    return (in8(CMOS_DATA) & 0x80);
}

char get_realtime_reg(int reg)
{
    out8(CMOS_ADDRESS, reg);
    return in8(CMOS_DATA);
}

/* --- Clock ---------------------------------------------------------------- */

void clock_time(time_t *time)
{
    CMOS_WAIT;

    time->second = from_bcd(get_realtime_reg(T_SECOND));
    time->minute = from_bcd(get_realtime_reg(T_MINUTE));
    time->hour = from_bcd(get_realtime_reg(T_HOUR));
    time->day = from_bcd(get_realtime_reg(T_DAY));
    time->month = from_bcd(get_realtime_reg(T_MONTH));
    time->year = from_bcd(get_realtime_reg(T_YEAR));
}

uint clock_read(time_selector_t selector)
{
    CMOS_WAIT;

    return from_bcd(get_realtime_reg(selector));
}
