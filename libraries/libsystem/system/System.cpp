
#include <libsystem/core/Plugs.h>

SystemInfo system_get_info()
{
    SystemInfo info;
    __plug_system_get_info(&info);
    return info;
}

SystemStatus system_get_status()
{
    SystemStatus status;
    __plug_system_get_status(&status);
    return status;
}

uint system_get_ticks()
{
    return __plug_system_get_ticks();
}
