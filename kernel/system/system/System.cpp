#include "system/Streams.h"
#include <libsystem/BuildInfo.h>

#include "archs/Arch.h"
#include "system/scheduling/Scheduler.h"
#include "system/system/System.h"

void system_hang()
{
    while (true)
    {
        Arch::halt();
    }
}

void system_stop()
{
    Arch::disable_interrupts();
    Kernel::logln("System stopped!");

    while (1)
    {
        Arch::disable_interrupts();
        Arch::halt();
    }
}

static uint32_t _system_tick;

void system_tick()
{
    if (_system_tick + 1 < _system_tick)
    {
        system_panic("System tick overflow!");
    }

    _system_tick++;
}

uint32_t system_get_tick()
{
    return _system_tick;
}

static TimeStamp _system_boot_timestamp = 0;

ElapsedTime system_get_uptime()
{
    return Arch::get_time() - _system_boot_timestamp;
}

void system_initialize()
{
    _system_boot_timestamp = Arch::get_time();
    Kernel::logln("hjert - " __BUILD_GITREF__);
}
