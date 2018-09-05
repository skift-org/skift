/* Copyright © 2018 MAKER.                                                    */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "cpu/cpu.h"
#include "kernel/clock.h"

/* --- CMOS/RTC ------------------------------------------------------------- */

#define CMOS_ADDRESS 0x70
#define CMOS_DATA 0x71

#define from_bcd(val) (((val) / 16) * 10 + ((val)&0xf))

bool is_cmos_update()
{
    outb(CMOS_ADDRESS, 0x0A);
    return (inb(CMOS_DATA) & 0x80);
}

char get_realtime_reg(int reg)
{
    outb(CMOS_ADDRESS, reg);
    return inb(CMOS_DATA);
}

/* --- Clock ---------------------------------------------------------------- */

void clock_time(time_t *time)
{
    while (is_cmos_update());

    time->second = from_bcd(get_realtime_reg(T_SECOND));
    time->minute = from_bcd(get_realtime_reg(T_MINUTE));
    time->hour   = from_bcd(get_realtime_reg(T_HOUR));
    time->day    = from_bcd(get_realtime_reg(T_DAY));
    time->month  = from_bcd(get_realtime_reg(T_MONTH));
    time->year   = from_bcd(get_realtime_reg(T_YEAR));
}

uint clock_read(time_selector_t selector)
{
    while (is_cmos_update());
    return from_bcd(get_realtime_reg(selector));
}