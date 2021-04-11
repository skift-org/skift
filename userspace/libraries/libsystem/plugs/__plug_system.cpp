
#include <abi/Syscalls.h>

#include <assert.h>
#include <libsystem/core/Plugs.h>

Tick __plug_system_get_ticks()
{
    Tick result = 0;
    assert(hj_system_tick(&result) == SUCCESS);
    return result;
}
