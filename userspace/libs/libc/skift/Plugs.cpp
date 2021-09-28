#include <stdio.h>
#include <string.h>

#include <abi/Syscalls.h>
#include <skift/Plugs.h>

#ifndef __KERNEL__

void __plug_assert_failed(const char *expr, const char *file, const char *function, int line)
{
    char buffer[256];
    snprintf(buffer, 256, "Assert failed: %s in %s:%s() ln%d!\n", expr, file, function, line);
    fwrite(buffer, strlen(buffer), 1, stdlog);
    abort();
}

#endif

TimeStamp __plug_system_get_time()
{
    TimeStamp timestamp = 0;
    assert(hj_system_time(&timestamp) == SUCCESS);
    return timestamp;
}