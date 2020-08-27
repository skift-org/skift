
#include <abi/Syscalls.h>

#include <libsystem/core/Plugs.h>

void __plug_system_get_info(SystemInfo *info)
{
    __syscall(SYS_SYSTEM_GET_INFO, (int)info);
}

void __plug_system_get_status(SystemStatus *status)
{
    __syscall(SYS_SYSTEM_GET_STATUS, (int)status);
}

TimeStamp __plug_system_get_time()
{
    TimeStamp timestamp = 0;
    __syscall(SYS_SYSTEM_GET_TIME, (int)&timestamp);
    return timestamp;
}

uint __plug_system_get_ticks()
{
    uint result = 0;
    __syscall(SYS_SYSTEM_GET_TICKS, (int)&result);
    return result;
}
