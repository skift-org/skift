#include "archs/Arch.h"
#include "archs/x86/COM.h"
#include "archs/x86/CPUID.h"
#include "archs/x86/FPU.h"
#include "archs/x86/RTC.h"
#include "archs/x86_64/GDT.h"
#include "archs/x86_64/Interrupts.h"
#include "archs/x86_64/Paging.h"
#include "archs/x86_64/x86_64.h"

namespace Arch
{

void disable_interrupts() { x86::cli(); }

void enable_interrupts() { x86::sti(); }

void halt()
{
    x86::hlt();
}

void yield()
{
    asm("int $127");
}

void save_context(Task *task)
{
    fpu_save_context(task);
}

void load_context(Task *task)
{
    fpu_load_context(task);
    x86_64::set_kernel_stack((uint64_t)task->kernel_stack + PROCESS_STACK_SIZE);
}

void task_go(Task *task)
{
    if (task->_flags & TASK_USER)
    {
        x86_64::InterruptStackFrame stackframe = {};

        stackframe.rsp = (uintptr_t)task->user_stack_pointer;

        stackframe.rflags = 0x202;
        stackframe.rip = (uintptr_t)task->entry_point;
        stackframe.rbp = (uintptr_t)stackframe.rsp;

        stackframe.cs = 0x1b;
        stackframe.ss = 0x23;

        task_kernel_stack_push(task, &stackframe, sizeof(x86_64::InterruptStackFrame));
    }
    else
    {
        x86_64::InterruptStackFrame stackframe = {};

        stackframe.rsp = (uintptr_t)task->kernel_stack_pointer - sizeof(x86_64::InterruptStackFrame);

        stackframe.rflags = 0x202;
        stackframe.rip = (uintptr_t)task->entry_point;
        stackframe.rbp = (uintptr_t)stackframe.rsp;

        stackframe.cs = 0x08;
        stackframe.ss = 0x10;

        task_kernel_stack_push(task, &stackframe, sizeof(x86_64::InterruptStackFrame));
    }
}

size_t debug_write(const void *buffer, size_t size)
{
    return com_write(COM1, buffer, size);
}

TimeStamp get_time()
{
    return rtc_now();
}

NO_RETURN void reboot()
{
    Kernel::logln("STUB {}", __func__);
    ASSERT_NOT_REACHED();
}

NO_RETURN void shutdown()
{
    Kernel::logln("STUB {}", __func__);
    ASSERT_NOT_REACHED();
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
        Kernel::logln("\t{016x}", stackframe->rip);
        stackframe = stackframe->rbp;
    }

    if (empty)
    {
        Kernel::logln("\t[EMPTY]");
    }
}

void dump_stack_frame(void *sfptr)
{
    auto stackframe = reinterpret_cast<x86_64::InterruptStackFrame *>(sfptr);

    Kernel::logln("\tRAX={016x} RBX={016x} RCX={016x}",
                  stackframe->rax,
                  stackframe->rbx,
                  stackframe->rcx);

    Kernel::logln("\tRDX={016x} RSI={016x} RDI={016x}",
                  stackframe->rdx,
                  stackframe->rsi,
                  stackframe->rdi);

    Kernel::logln("\tR08={016x} R09={016x} R10={016x}",
                  stackframe->r8,
                  stackframe->r9,
                  stackframe->r10);

    Kernel::logln("\tR11={016x} R12={016x} R13={016x}",
                  stackframe->r11,
                  stackframe->r12,
                  stackframe->r13);

    Kernel::logln("\tR14={016x} R15={016x} RBP={016x}",
                  stackframe->r14,
                  stackframe->r15,
                  stackframe->rbp);

    Kernel::logln("");

    Kernel::logln("\tINT={08x} ERR={08x}\n", stackframe->intno, stackframe->err);

    Kernel::logln("");

    Kernel::logln("\tRIP={016x}  CS={016x} FLG={016x}\n"
                  "\tRSP={016x}  SS={016x}",
                  stackframe->rip,
                  stackframe->cs,
                  stackframe->rflags,
                  stackframe->rsp,
                  stackframe->ss);

    Kernel::logln("\tCR0={016x} CR2={016x} CR3={016x} CR4={016x}", x86::CR0(), x86::CR2(), x86::CR3(), x86::CR4());

    Kernel::logln("\n\tBacktrace:");
    backtrace_internal(stackframe->rbp);
}

void backtrace()
{
    backtrace_internal(x86_64::RBP());
}

void *kernel_address_space()
{
    return x86_64::kernel_address_space();
}

void virtual_initialize()
{
    return x86_64::virtual_initialize();
}

void virtual_memory_enable()
{
    return x86_64::virtual_memory_enable();
}

bool virtual_present(void *address_space, uintptr_t virtual_address)
{
    return x86_64::virtual_present(address_space, virtual_address);
}

uintptr_t virtual_to_physical(void *address_space, uintptr_t virtual_address)
{
    return x86_64::virtual_to_physical(address_space, virtual_address);
}

Result virtual_map(void *address_space, MemoryRange physical_range, uintptr_t virtual_address, MemoryFlags flags)
{
    return x86_64::virtual_map(address_space, physical_range, virtual_address, flags);
}

MemoryRange virtual_alloc(void *address_space, MemoryRange physical_range, MemoryFlags flags)
{
    return x86_64::virtual_alloc(address_space, physical_range, flags);
}

void virtual_free(void *address_space, MemoryRange virtual_range)
{
    return x86_64::virtual_free(address_space, virtual_range);
}

void *address_space_create()
{
    return x86_64::address_space_create();
}

void address_space_destroy(void *address_space)
{
    return x86_64::address_space_destroy(address_space);
}

void address_space_switch(void *address_space)
{
    return x86_64::address_space_switch(address_space);
}

} // namespace Arch
