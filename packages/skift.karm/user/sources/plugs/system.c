#include <skift/__plugs__.h>
#include <skift/syscalls.h>

void __system_get_info(system_info_t* info)
{
    __syscall(SYS_SYSTEM_GET_INFO, (int)info, 0, 0, 0, 0);
}