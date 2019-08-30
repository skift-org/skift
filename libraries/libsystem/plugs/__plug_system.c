/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/__plugs__.h>
#include <libkernel/syscalls.h>

void __plug_system_get_info(system_info_t *info)
{
    __syscall(SYS_SYSTEM_GET_INFO, (int)info, 0, 0, 0, 0);
}

void __plug_system_get_status(system_status_t *status)
{
    __syscall(SYS_SYSTEM_GET_STATUS, (int)status, 0, 0, 0, 0);
}

timestamp_t __plug_system_get_time(void)
{
    timestamp_t timestamp = 0;
    __syscall(SYS_SYSTEM_GET_TIME, (int)&timestamp, 0,0,0,0);
    return timestamp;
}