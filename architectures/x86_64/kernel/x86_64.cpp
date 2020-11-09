#include <libsystem/Assert.h>
#include <libsystem/Logger.h>
#include <libsystem/core/Plugs.h>
#include <libsystem/io/Stream.h>

#include "architectures/x86/kernel/COM.h"
#include "architectures/x86/kernel/CPUID.h"
#include "architectures/x86/kernel/IOPort.h"
#include "architectures/x86/kernel/PIC.h"
#include "architectures/x86/kernel/PIT.h"
#include "architectures/x86/kernel/RTC.h"

#include "architectures/x86_64/kernel/GDT.h"
#include "architectures/x86_64/kernel/IDT.h"
#include "architectures/x86_64/kernel/Interrupts.h"
#include "architectures/x86_64/kernel/x86_64.h"

#include "kernel/graphics/Graphics.h"
#include "kernel/handover/Handover.h"
#include "kernel/interrupts/Interupts.h"
#include "kernel/system/System.h"

extern "C" void arch_main(void *info, uint32_t magic)
{
    __plug_init();

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
    ASSERT_NOT_REACHED();
}

void arch_save_context(Task *task)
{
    __unused(task);

    ASSERT_NOT_REACHED();
}

void arch_load_context(Task *task)
{
    __unused(task);

    ASSERT_NOT_REACHED();
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

    printf("\tRAX=%016x RBX=%016x RCX=%016x \n",
           stackframe->rax,
           stackframe->rbx,
           stackframe->rcx);

    printf("\tRDX=%016x RSI=%016x RDI=%016x\n",
           stackframe->rdx,
           stackframe->rsi,
           stackframe->rdi);

    printf("\tR08=%016x R09=%016x R10=%016x\n",
           stackframe->r8,
           stackframe->r9,
           stackframe->r10);

    printf("\tR11=%016x R12=%016x R13=%016x\n",
           stackframe->r11,
           stackframe->r12,
           stackframe->r13);

    printf("\tR14=%016x R15=%016x RBP=%016x\n",
           stackframe->r14,
           stackframe->r15,
           stackframe->rbp);

    printf("\n");

    printf("\tINT=%08x ERR=%08x\n", stackframe->intno, stackframe->err);

    printf("\n");

    printf("\tRIP=%016x  CS=%016x FLG=%016x\n"
           "\tRSP=%016x  SS=%016x\n",
           stackframe->rip,
           stackframe->cs,
           stackframe->rflags,
           stackframe->rsp,
           stackframe->ss);

    printf("\tCR0=%016x CR2=%016x CR3=%016x CR4=%016x\n", CR0(), CR2(), CR3(), CR4());

    printf("\n\tBacktrace:\n");
    backtrace_internal(stackframe->rbp);
}

void arch_backtrace()
{
    backtrace_internal(RBP());
}
