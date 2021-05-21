#include "system/Streams.h"

#include "system/interrupts/Dispatcher.h"
#include "system/interrupts/Interupts.h"
#include "system/scheduling/Scheduler.h"
#include "system/system/System.h"
#include "system/tasking/Syscalls.h"

#include "archs/x86/PIC.h"

#include "archs/x86_64/Interrupts.h"
#include "archs/x86_64/x86_64.h"

namespace Arch::x86_64
{

static const char *_exception_messages[32] = {
    "Division by zero",
    "Debug",
    "Non-maskable interrupt",
    "Breakpoint",
    "Detected overflow",
    "Out-of-bounds",
    "Invalid opcode",
    "No coprocessor",
    "Double fault",
    "Coprocessor segment overrun",
    "Bad TSS",
    "Segment not present",
    "Stack fault",
    "General protection fault",
    "Page fault",
    "Unknown interrupt",
    "Coprocessor fault",
    "Alignment check",
    "Machine check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
};

extern "C" uint64_t interrupts_handler(uintptr_t rsp)
{
    InterruptStackFrame *stackframe = reinterpret_cast<InterruptStackFrame *>(rsp);

    if (stackframe->intno < 32)
    {
        if (stackframe->cs == 0x1B)
        {
            Kernel::logln("Task {}({}) triggered an exception: '{}' {x}.{x} (IP={08x} CR2={08x})",
                          scheduler_running()->name,
                          scheduler_running_id(),
                          _exception_messages[stackframe->intno],
                          stackframe->intno,
                          stackframe->err,
                          stackframe->rip,
                          x86::CR2());

            task_dump(scheduler_running());
            Arch::dump_stack_frame(stackframe);

            x86::sti();
            scheduler_running()->cancel(-1);
        }
        else
        {
            system_panic_with_context(
                stackframe,
                "CPU EXCEPTION: '{}' (INT:{} ERR:{x}) !",
                _exception_messages[stackframe->intno],
                stackframe->intno,
                stackframe->err);
        }
    }
    else if (stackframe->intno < 48)
    {
        interrupts_disable_holding();

        int irq = stackframe->intno - 32;

        if (irq == 0)
        {
            system_tick();
            rsp = schedule(rsp);
        }
        else
        {
            dispatcher_dispatch(irq);
        }

        interrupts_enable_holding();
    }
    else if (stackframe->intno == 127)
    {
        interrupts_disable_holding();

        rsp = schedule(rsp);

        interrupts_enable_holding();
    }
    else if (stackframe->intno == 128)
    {
        x86::sti();

        stackframe->rax = task_do_syscall(
            (Syscall)stackframe->rax,
            stackframe->rbx,
            stackframe->rcx,
            stackframe->rdx,
            stackframe->rsi,
            stackframe->rdi);

        x86::cli();
    }

    pic_ack(stackframe->intno);

    return rsp;
}

} // namespace Arch::x86_64
