#include <libsystem/Logger.h>

#include "kernel/interrupts/Dispatcher.h"
#include "kernel/interrupts/Interupts.h"
#include "kernel/scheduling/Scheduler.h"
#include "kernel/system/System.h"
#include "kernel/tasking/Syscalls.h"

#include "archs/x86/kernel/PIC.h"

#include "archs/x86_64/kernel/Interrupts.h"
#include "archs/x86_64/kernel/x86_64.h"

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
            logger_error("Task %s(%d) triggered an exception: '%s' %x.%x (IP=%08x CR2=%08x)",
                         scheduler_running()->name,
                         scheduler_running_id(),
                         _exception_messages[stackframe->intno],
                         stackframe->intno,
                         stackframe->err,
                         stackframe->rip,
                         CR2());

            task_dump(scheduler_running());
            arch_dump_stack_frame(stackframe);

            sti();
            scheduler_running()->cancel(-1);
        }
        else
        {
            system_panic_with_context(
                stackframe,
                "CPU EXCEPTION: '%s' (INT:%d ERR:%x) !",
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
        sti();

        stackframe->rax = task_do_syscall(
            (Syscall)stackframe->rax,
            stackframe->rbx,
            stackframe->rcx,
            stackframe->rdx,
            stackframe->rsi,
            stackframe->rdi);

        cli();
    }

    pic_ack(stackframe->intno);

    return rsp;
}
