#include <libsystem/Logger.h>

#include "archs/Architectures.h"

#include "kernel/interrupts/Dispatcher.h"
#include "kernel/interrupts/Interupts.h"

static bool _can_be_holded = false;
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
    return !_can_be_holded || _depth > 0;
}

void interrupts_enable_holding()
{
    _can_be_holded = true;
}

void interrupts_disable_holding()
{
    _can_be_holded = false;
}

void interrupts_retain()
{
    if (_can_be_holded)
    {
        arch_disable_interrupts();
        _depth++;
    }
}

void interrupts_release()
{
    if (_can_be_holded)
    {
        assert(_depth > 0);
        _depth--;

        if (_depth == 0)
        {
            arch_enable_interrupts();
        }
    }
}
