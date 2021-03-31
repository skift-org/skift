#include <libsystem/BuildInfo.h>
#include <libsystem/Logger.h>

#include "archs/Architectures.h"
#include "kernel/scheduling/Scheduler.h"
#include "kernel/system/System.h"

void system_hang()
{
    while (true)
    {
        arch_halt();
    }
}

void system_stop()
{
    arch_disable_interrupts();
    logger_info("System stopped!");

    while (1)
    {
        arch_disable_interrupts();
        arch_halt();
    }
}

static uint32_t _system_tick;

void system_tick()
{
    if (_system_tick + 1 < _system_tick)
    {
        logger_fatal("System tick overflow!");
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
    return arch_get_time() - _system_boot_timestamp;
}

void system_initialize()
{
    _system_boot_timestamp = arch_get_time();
    logger_info("hjert - " __BUILD_GITREF__);
}
