#include <assert.h>
#include <libsystem/Logger.h>

#include "kernel/interrupts/Dispatcher.h"
#include "kernel/interrupts/Interupts.h"
#include "kernel/scheduling/Scheduler.h"
#include "kernel/system/System.h"
#include "kernel/tasking/Syscalls.h"

#include "archs/x86/kernel/PIC.h"
#include "archs/x86_32/kernel/Interrupts.h"
#include "archs/x86_32/kernel/x86_32.h"

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

extern "C" uint32_t interrupts_handler(uintptr_t esp, InterruptStackFrame stackframe)
{
    if (stackframe.intno < 32)
    {
        if (stackframe.cs == 0x1B)
        {
            sti();

            logger_error("Task %s(%d) triggered an exception: '%s' %x.%x (IP=%08x CR2=%08x)",
                         scheduler_running()->name,
                         scheduler_running_id(),
                         _exception_messages[stackframe.intno],
                         stackframe.intno,
                         stackframe.err,
                         stackframe.eip,
                         CR2());

            task_dump(scheduler_running());
            arch_dump_stack_frame(reinterpret_cast<void *>(&stackframe));

            scheduler_running()->cancel(-1);
        }
        else
        {
            system_panic_with_context(
                &stackframe,
                "CPU EXCEPTION: '%s' (INT:%d ERR:%x) !",
                _exception_messages[stackframe.intno],
                stackframe.intno,
                stackframe.err);
        }
    }
    else if (stackframe.intno < 48)
    {
        interrupts_disable_holding();

        int irq = stackframe.intno - 32;

        if (irq == 0)
        {
            system_tick();
            esp = schedule(esp);
        }
        else
        {
            dispatcher_dispatch(irq);
        }

        interrupts_enable_holding();
    }
    else if (stackframe.intno == 127)
    {
        interrupts_disable_holding();

        esp = schedule(esp);

        interrupts_enable_holding();
    }
    else if (stackframe.intno == 128)
    {
        sti();

        if (stackframe.eax == HJ_PROCESS_CLONE)
        {
            InterruptsRetainer retainer;

            auto usf = ((UserInterruptStackFrame *)&stackframe);

            *((int *)stackframe.ebx) = 0;

            auto child = task_clone(scheduler_running(), usf->user_esp, usf->eip);

            *((int *)stackframe.ebx) = child->id;

            stackframe.eax = SUCCESS;
        }
        else
        {
            stackframe.eax = task_do_syscall((Syscall)stackframe.eax, stackframe.ebx, stackframe.ecx, stackframe.edx, stackframe.esi, stackframe.edi);
        }

        cli();
    }

    pic_ack(stackframe.intno);

    return esp;
}
