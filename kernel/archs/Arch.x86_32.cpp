#include "archs/x86/COM.h"
#include "archs/x86/CPUID.h"
#include "archs/x86/FPU.h"
#include "archs/x86/Power.h"
#include "archs/x86/RTC.h"
#include "archs/x86/x86.h"
#include "archs/x86_32/GDT.h"
#include "archs/x86_32/Interrupts.h"
#include "archs/x86_32/Paging.h"
#include "archs/x86_32/x86_32.h"

#include "system/Streams.h"
#include "system/graphics/EarlyConsole.h"

namespace Arch
{

void disable_interrupts() { x86::cli(); }

void enable_interrupts() { x86::sti(); }

void halt() { x86::hlt(); }

void yield() { asm("int $127"); }

void save_context(Task *task)
{
    fpu_save_context(task);
}

void load_context(Task *task)
{
    fpu_load_context(task);
    x86_32::set_kernel_stack((uintptr_t)task->kernel_stack + PROCESS_STACK_SIZE);
}

void task_go(Task *task)
{
    fpu_init_context(task);

    if (task->_flags & TASK_USER)
    {
        x86_32::UserInterruptStackFrame stackframe = {};

        stackframe.user_esp = task->user_stack_pointer;

        stackframe.eflags = 0x202;
        stackframe.eip = (uintptr_t)task->entry_point;
        stackframe.ebp = 0;

        stackframe.cs = 0x1b;
        stackframe.ds = 0x23;
        stackframe.es = 0x23;
        stackframe.fs = 0x23;
        stackframe.gs = 0x23;
        stackframe.ss = 0x23;

        task_kernel_stack_push(task, &stackframe, sizeof(x86_32::UserInterruptStackFrame));
    }
    else
    {
        x86_32::InterruptStackFrame stackframe = {};

        stackframe.eflags = 0x202;
        stackframe.eip = (uintptr_t)task->entry_point;
        stackframe.ebp = 0;

        stackframe.cs = 0x08;
        stackframe.ds = 0x10;
        stackframe.es = 0x10;
        stackframe.fs = 0x10;
        stackframe.gs = 0x10;

        task_kernel_stack_push(task, &stackframe, sizeof(x86_32::InterruptStackFrame));
    }
}

size_t debug_write(const void *buffer, size_t size) { return com_write(COM1, buffer, size); }

TimeStamp get_time() { return rtc_now(); }

NO_RETURN void reboot()
{
    early_console_enable();
    Kernel::logln("Rebooting...");

    x86::reboot_8042();
    x86::reboot_triple_fault();

    Kernel::logln("Failed to reboot: Halting!");
    system_stop();
}

NO_RETURN void shutdown()
{
    early_console_enable();
    Kernel::logln("Shutting down...");

    x86::shutdown_virtual_machines();
    x86::shutdown_acpi();

    Kernel::logln("Failed to shutdown: Halting!");
    system_stop();
}

void panic_dump()
{
    cpuid_dump();
}

struct Stackframe
{
    Stackframe *ebp;
    uint32_t eip;
};

void backtrace_internal(uint32_t ebp)
{
    bool empty = true;
    Stackframe *stackframe = reinterpret_cast<Stackframe *>(ebp);

    while (stackframe)
    {
        empty = false;
        stream_format(log_stream, "\t%08x\n", stackframe->eip);
        stackframe = stackframe->ebp;
    }

    if (empty)
    {
        stream_format(log_stream, "\t[EMPTY]\n");
    }
}

void dump_stack_frame(void *sf)
{
    auto stackframe = reinterpret_cast<x86_32::InterruptStackFrame *>(sf);

    stream_format(out_stream, "\tCS=%04x DS=%04x ES=%04x FS=%04x GS=%04x\n", stackframe->cs, stackframe->ds, stackframe->es, stackframe->fs, stackframe->gs);
    stream_format(out_stream, "\tEAX=%08x EBX=%08x ECX=%08x EDX=%08x\n", stackframe->eax, stackframe->ebx, stackframe->ecx, stackframe->edx);
    stream_format(out_stream, "\tEDI=%08x ESI=%08x EBP=%08x ESP=%08x\n", stackframe->edi, stackframe->esi, stackframe->ebp, stackframe->esp);
    stream_format(out_stream, "\tINT=%08x ERR=%08x EIP=%08x FLG=%08x\n", stackframe->intno, stackframe->err, stackframe->eip, stackframe->eflags);

    stream_format(out_stream, "\tCR0=%08x CR2=%08x CR3=%08x CR4=%08x\n", x86::CR0(), x86::CR2(), x86::CR3(), x86::CR4());

    stream_format(out_stream, "\n\tBacktrace:\n");
    backtrace_internal(stackframe->ebp);
}

void backtrace()
{
    backtrace_internal(x86_32::EBP());
}

void *kernel_address_space()
{
    return x86_32::kernel_address_space();
}

void virtual_initialize()
{
    return x86_32::virtual_initialize();
}

void virtual_memory_enable()
{
    return x86_32::virtual_memory_enable();
}

bool virtual_present(void *address_space, uintptr_t virtual_address)
{
    return x86_32::virtual_present(address_space, virtual_address);
}

uintptr_t virtual_to_physical(void *address_space, uintptr_t virtual_address)
{
    return x86_32::virtual_to_physical(address_space, virtual_address);
}

Result virtual_map(void *address_space, MemoryRange physical_range, uintptr_t virtual_address, MemoryFlags flags)
{
    return x86_32::virtual_map(address_space, physical_range, virtual_address, flags);
}

MemoryRange virtual_alloc(void *address_space, MemoryRange physical_range, MemoryFlags flags)
{
    return x86_32::virtual_alloc(address_space, physical_range, flags);
}

void virtual_free(void *address_space, MemoryRange virtual_range)
{
    return x86_32::virtual_free(address_space, virtual_range);
}

void *address_space_create()
{
    return x86_32::address_space_create();
}

void address_space_destroy(void *address_space)
{
    return x86_32::address_space_destroy(address_space);
}

void address_space_switch(void *address_space)
{
    return x86_32::address_space_switch(address_space);
}

} // namespace Arch
