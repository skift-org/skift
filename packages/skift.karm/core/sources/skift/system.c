#include <skift/__plugs__.h>
#include <skift/cstring.h>
#include <skift/system.h>

static bool system_info_cached = false;
static system_info_t system_info_cache = {0};

void system_get_info(system_info_t* info)
{
    if (!system_info_cached)
    {
        __plug_system_get_info(&system_info_cache);
        system_info_cached = true;
    }

    memcpy(info, &system_info_cache, sizeof(system_info_t));
}

void system_get_status(system_status_t* status)
{
    __plug_system_get_status(status);
}
