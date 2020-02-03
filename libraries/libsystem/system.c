/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/__plugs__.h>
#include <libsystem/system.h>

SystemInfo system_get_info(void)
{
    SystemInfo info;
    __plug_system_get_info(&info);
    return info;
}

SystemStatus system_get_status(void)
{
    SystemStatus status;
    __plug_system_get_status(&status);
    return status;
}

uint system_get_ticks(void)
{
    return __plug_system_get_ticks();
}
