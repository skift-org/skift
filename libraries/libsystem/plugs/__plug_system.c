/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <abi/Syscalls.h>

#include <libsystem/__plugs__.h>

void __plug_system_get_info(SystemInfo *info)
{
    __syscall(SYS_SYSTEM_GET_INFO, (int)info, 0, 0, 0, 0);
}

void __plug_system_get_status(SystemStatus *status)
{
    __syscall(SYS_SYSTEM_GET_STATUS, (int)status, 0, 0, 0, 0);
}

TimeStamp __plug_system_get_time(void)
{
    TimeStamp timestamp = 0;
    __syscall(SYS_SYSTEM_GET_TIME, (int)&timestamp, 0, 0, 0, 0);
    return timestamp;
}

uint __plug_system_get_ticks()
{
    return __syscall(SYS_SYSTEM_GET_TICKS, 0, 0, 0, 0, 0);
}