#include <libsystem/Logger.h>

#include "arch/x86/kernel/PIC.h"
#include "arch/x86_64/kernel/Interrupts.h"

extern "C" uint64_t interrupts_handler(uintptr_t rsp)
{
    InterruptStackFrame *stackframe = reinterpret_cast<InterruptStackFrame *>(rsp);
    __unused(stackframe);

    logger_trace("Interrupt %d!", stackframe->intno);

    pic_ack(stackframe->intno);

    return rsp;
}
