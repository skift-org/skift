/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/__plugs__.h>
#include <skift/cstring.h>
#include <skift/system.h>

void system_get_info(system_info_t *info)
{
    __plug_system_get_info(info);
}

void system_get_status(system_status_t *status)
{
    __plug_system_get_status(status);
}
