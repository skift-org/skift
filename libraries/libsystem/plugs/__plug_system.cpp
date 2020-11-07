
#include <abi/Syscalls.h>

#include <libsystem/Assert.h>
#include <libsystem/core/Plugs.h>

TimeStamp __plug_system_get_time()
{
    TimeStamp timestamp = 0;
    assert(hj_system_time(&timestamp) == SUCCESS);
    return timestamp;
}

uint __plug_system_get_ticks()
{
    uint result = 0;
    assert(hj_system_tick(&result) == SUCCESS);
    return result;
}
