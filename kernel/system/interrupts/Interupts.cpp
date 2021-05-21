#include "system/Streams.h"

#include "archs/Arch.h"

#include "system/interrupts/Dispatcher.h"
#include "system/interrupts/Interupts.h"

static bool _can_be_holded = false;
static int _depth = 0;

void interrupts_initialize()
{
    dispatcher_initialize();

    Kernel::logln("Enabling interrupts!");

    interrupts_enable_holding();
    Arch::enable_interrupts();
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
        Arch::disable_interrupts();
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
            Arch::enable_interrupts();
        }
    }
}
