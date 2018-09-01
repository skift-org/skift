#include "cpu/cpu.h"
#include "kernel/time.h"
#include "kernel/version.h"

#define CMOS_ADDRESS 0x70
#define CMOS_DATA 0x71

#define from_bcd(val)  ((val / 16) * 10 + (val & 0xf))

bool is_cmos_update()
{
    outb(CMOS_ADDRESS, 0x0A);
    return (inb(CMOS_DATA) & 0x80);
}

u8 get_realtime_reg(s32 reg)
{
    outb(CMOS_ADDRESS, reg);
    return inb(CMOS_DATA);
}

time_t time()
{
    // Waite for cmos update.
    while (is_cmos_update());
    time_t time;

    time.second = from_bcd(get_realtime_reg(TIME_SECOND));
    time.minute = from_bcd(get_realtime_reg(TIME_MINUTE));
    time.hour = from_bcd(get_realtime_reg(TIME_HOUR));
    time.day = from_bcd(get_realtime_reg(TIME_DAY));
    time.month = from_bcd(get_realtime_reg(TIME_MONTH));
    time.year = from_bcd(get_realtime_reg(TIME_YEAR));

    return time;
}

u32 time_get(u32 selector)
{
    while (is_cmos_update());
    return from_bcd(get_realtime_reg(selector));
}


#include <stdio.h>
#include <string.h>
#include "devices/vga.h"
#include "utils.h"


void* time_task(void* arg)
{
    UNUSED(arg);

    char buffer[80];

    while(true)
    {
        memset(buffer, 0, 80);
        sprintf(buffer, " [ %s '%s' ] [ %d:%d:%d ]",
        __kernel_name, __kernel_version_codename, 
        time_get(TIME_HOUR), time_get(TIME_MINUTE), time_get(TIME_SECOND));
    
        for(u32 i = 0; i < 80; i++)
        {
            vga_cell(i, 0, vga_entry(buffer[i], vga_white, vga_gray));  
        }
    }

    return NULL;
}