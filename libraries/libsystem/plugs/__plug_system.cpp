
#include <abi/Syscalls.h>

#include <libsystem/core/Plugs.h>

void __plug_system_get_info(SystemInfo *info)
{
    __syscall(SYS_SYSTEM_GET_INFO, (int)info, 0, 0, 0, 0);
}

void __plug_system_get_status(SystemStatus *status)
{
    __syscall(SYS_SYSTEM_GET_STATUS, (int)status, 0, 0, 0, 0);
}

TimeStamp __plug_system_get_time()
{
    TimeStamp timestamp = 0;
    __syscall(SYS_SYSTEM_GET_TIME, (int)&timestamp, 0, 0, 0, 0);
    return timestamp;
}

uint __plug_system_get_ticks()
{
    uint result = 0;
    __syscall(SYS_SYSTEM_GET_TICKS, (int)&result, 0, 0, 0, 0);
    return result;
}
