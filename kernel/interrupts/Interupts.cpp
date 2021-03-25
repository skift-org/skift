#include <libsystem/Logger.h>

#include "archs/Architectures.h"

#include "kernel/interrupts/Dispatcher.h"
#include "kernel/interrupts/Interupts.h"

static bool _holded = false;
static int _depth = 0;

void interrupts_initialize()
{
    dispatcher_initialize();

    logger_info("Enabling interrupts!");

    interrupts_enable_holding();
    arch_enable_interrupts();
}

bool interrupts_retained()
{
    return !_holded || _depth > 0;
}

void interrupts_enable_holding()
{
    _holded = true;
}

void interrupts_disable_holding()
{
    _holded = false;
}

void interrupts_retain()
{
    if (_holded)
    {
        arch_disable_interrupts();
        _depth++;
    }
}

void interrupts_release()
{
    if (_holded)
    {
        _depth--;

        if (_depth == 0)
            arch_enable_interrupts();
    }
}
