#include <libsystem/thread/Atomic.h>

#include "architectures/Architectures.h"
#include "kernel/interrupts/Dispatcher.h"
#include "kernel/interrupts/Interupts.h"

void interrupts_initialize()
{
    dispatcher_initialize();
    atomic_enable();
    arch_enable_interrupts();
}
