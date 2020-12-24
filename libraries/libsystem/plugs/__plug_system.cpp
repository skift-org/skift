
#include <abi/Syscalls.h>

#include <libsystem/Assert.h>
#include <libsystem/core/Plugs.h>

TimeStamp __plug_system_get_time()
{
    TimeStamp timestamp = 0;
    assert(hj_system_time(&timestamp) == SUCCESS);
    return timestamp;
}

Tick __plug_system_get_ticks()
{
    Tick result = 0;
    assert(hj_system_tick(&result) == SUCCESS);
    return result;
}
