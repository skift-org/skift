#include <assert.h>
#include <libsystem/Logger.h>
#include <libsystem/core/Plugs.h>
#include <libsystem/io/Stream.h>

#include "kernel/graphics/Graphics.h"
#include "kernel/handover/Handover.h"
#include "kernel/interrupts/Interupts.h"
#include "kernel/system/System.h"
#include "kernel/tasking/Task.h"

#include "archs/x86/kernel/COM.h"
#include "archs/x86/kernel/CPUID.h"
#include "archs/x86/kernel/FPU.h"
#include "archs/x86/kernel/IOPort.h"
#include "archs/x86/kernel/PIC.h"
#include "archs/x86/kernel/PIT.h"
#include "archs/x86/kernel/RTC.h"

#include "archs/x86_64/kernel/GDT.h"
#include "archs/x86_64/kernel/IDT.h"
#include "archs/x86_64/kernel/Interrupts.h"
#include "archs/x86_64/kernel/x86_64.h"

extern "C" void arch_main(void *info, uint32_t magic)
{
    __plug_initialize();

    com_initialize(COM1);
    com_initialize(COM2);
    com_initialize(COM3);
    com_initialize(COM4);

    auto handover = handover_initialize(info, magic);

    graphic_early_initialize(handover);

    if (handover->memory_usable < 127 * 1024)
    {
        system_panic("No enoughs memory (%uKio)!", handover->memory_usable / 1024);
    }

    gdt_initialize();
    idt_initialize();
    pic_initialize();
    fpu_initialize();
    pit_initialize(1000);

    system_main(handover);

    ASSERT_NOT_REACHED();
}

void arch_disable_interrupts() { cli(); }

void arch_enable_interrupts() { sti(); }

void arch_halt()
{
    hlt();
}

void arch_yield()
{
    asm("int $127");
}

void arch_save_context(Task *task)
{
    fpu_save_context(task);
}

void arch_load_context(Task *task)
{
    fpu_load_context(task);
    set_kernel_stack((uint64_t)task->kernel_stack + PROCESS_STACK_SIZE);
}

void arch_task_go(Task *task)
{
    if (task->_flags & TASK_USER)
    {
        InterruptStackFrame stackframe = {};

        stackframe.rsp = (uintptr_t)task->user_stack_pointer;

        stackframe.rflags = 0x202;
        stackframe.rip = (uintptr_t)task->entry_point;
        stackframe.rbp = (uintptr_t)stackframe.rsp;

        stackframe.cs = 0x1b;
        stackframe.ss = 0x23;

        task_kernel_stack_push(task, &stackframe, sizeof(InterruptStackFrame));
    }
    else
    {
        InterruptStackFrame stackframe = {};

        stackframe.rsp = (uintptr_t)task->kernel_stack_pointer - sizeof(InterruptStackFrame);

        stackframe.rflags = 0x202;
        stackframe.rip = (uintptr_t)task->entry_point;
        stackframe.rbp = (uintptr_t)stackframe.rsp;

        stackframe.cs = 0x08;
        stackframe.ss = 0x10;

        task_kernel_stack_push(task, &stackframe, sizeof(InterruptStackFrame));
    }
}

size_t arch_debug_write(const void *buffer, size_t size)
{
    return com_write(COM1, buffer, size);
}

TimeStamp arch_get_time()
{
    return rtc_now();
}

__no_return void arch_reboot()
{
    logger_warn("STUB %s", __func__);
    ASSERT_NOT_REACHED();
}

__no_return void arch_shutdown()
{
    logger_warn("STUB %s", __func__);
    ASSERT_NOT_REACHED();
}

void arch_panic_dump()
{
    cpuid_dump();
}

struct Stackframe
{
    Stackframe *rbp;
    uint64_t rip;
};

void backtrace_internal(uint64_t rbp)
{
    bool empty = true;
    Stackframe *stackframe = reinterpret_cast<Stackframe *>(rbp);

    while (stackframe)
    {
        empty = false;
        stream_format(log_stream, "\t%016x\n", stackframe->rip);
        stackframe = stackframe->rbp;
    }

    if (empty)
    {
        stream_format(log_stream, "\t[EMPTY]\n");
    }
}

void arch_dump_stack_frame(void *sfptr)
{
    auto stackframe = reinterpret_cast<InterruptStackFrame *>(sfptr);

    stream_format(out_stream, "\tRAX=%016x RBX=%016x RCX=%016x \n",
                  stackframe->rax,
                  stackframe->rbx,
                  stackframe->rcx);

    stream_format(out_stream, "\tRDX=%016x RSI=%016x RDI=%016x\n",
                  stackframe->rdx,
                  stackframe->rsi,
                  stackframe->rdi);

    stream_format(out_stream, "\tR08=%016x R09=%016x R10=%016x\n",
                  stackframe->r8,
                  stackframe->r9,
                  stackframe->r10);

    stream_format(out_stream, "\tR11=%016x R12=%016x R13=%016x\n",
                  stackframe->r11,
                  stackframe->r12,
                  stackframe->r13);

    stream_format(out_stream, "\tR14=%016x R15=%016x RBP=%016x\n",
                  stackframe->r14,
                  stackframe->r15,
                  stackframe->rbp);

    stream_format(out_stream, "\n");

    stream_format(out_stream, "\tINT=%08x ERR=%08x\n", stackframe->intno, stackframe->err);

    stream_format(out_stream, "\n");

    stream_format(out_stream, "\tRIP=%016x  CS=%016x FLG=%016x\n"
                              "\tRSP=%016x  SS=%016x",
                  stackframe->rip,
                  stackframe->cs,
                  stackframe->rflags,
                  stackframe->rsp,
                  stackframe->ss);

    stream_format(out_stream, " CR0=%016x\n\tCR2=%016x CR3=%016x CR4=%016x\n", CR0(), CR2(), CR3(), CR4());

    stream_format(out_stream, "\n\tBacktrace:\n");
    backtrace_internal(stackframe->rbp);
}

void arch_backtrace()
{
    backtrace_internal(RBP());
}
