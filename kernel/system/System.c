#include <libsystem/BuildInfo.h>
#include <libsystem/Logger.h>

#include "arch/Arch.h"
#include "kernel/system/System.h"

void system_hang(void)
{
    while (true)
    {
        arch_halt();
    }
}

void system_stop(void)
{
    while (1)
    {
        arch_disable_interupts();
        arch_halt();
    }
}

static uint32_t _system_tick;

void system_tick(void)
{
    if (_system_tick + 1 < _system_tick)
    {
        system_panic("System tick overflow!");
    }

    _system_tick++;
}

uint32_t system_get_tick(void)
{
    return _system_tick;
}

static TimeStamp _system_boot_timestamp = 0;

ElapsedTime system_get_uptime(void)
{
    return arch_get_time() - _system_boot_timestamp;
}

void system_initialize(void)
{
    _system_boot_timestamp = arch_get_time();
    logger_info("hjert - " __BUILD_GITREF__);
}
