#include <skift/__plugs__.h>
#include <hjert/shared/syscalls.h>

void __plug_system_get_info(system_info_t* info)
{
    __syscall(SYS_SYSTEM_GET_INFO, (int)info, 0, 0, 0, 0);
}

void __plug_system_get_status(system_status_t* status)
{
    __syscall(SYS_SYSTEM_GET_STATUS, (int)status, 0, 0, 0, 0);
}