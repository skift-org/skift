#include <libsystem/Assert.h>
#include <libsystem/core/Plugs.h>

#include "archs/x86/kernel/COM.h"
#include "archs/x86/kernel/CPUID.h"
#include "archs/x86/kernel/FPU.h"
#include "archs/x86/kernel/PIC.h"
#include "archs/x86/kernel/PIT.h"
#include "archs/x86/kernel/RTC.h"
#include "archs/x86_32/kernel/ACPI.h"
#include "archs/x86_32/kernel/GDT.h"
#include "archs/x86_32/kernel/IDT.h"
#include "archs/x86_32/kernel/Interrupts.h"
#include "archs/x86_32/kernel/LAPIC.h"
#include "archs/x86_32/kernel/x86_32.h"

#include "kernel/firmware/SMBIOS.h"
#include "kernel/graphics/EarlyConsole.h"
#include "kernel/graphics/Graphics.h"
#include "kernel/system/System.h"

void arch_disable_interrupts() { cli(); }

void arch_enable_interrupts() { sti(); }

void arch_halt() { hlt(); }

void arch_yield() { asm("int $127"); }

void arch_save_context(Task *task)
{
    fpu_save_context(task);
}

void arch_load_context(Task *task)
{
    fpu_load_context(task);
    set_kernel_stack((uintptr_t)task->kernel_stack + PROCESS_STACK_SIZE);
}

void arch_task_go(Task *task)
{
    if (task->user)
    {
        UserInterruptStackFrame stackframe = {};

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

        task_kernel_stack_push(task, &stackframe, sizeof(UserInterruptStackFrame));
    }
    else
    {
        InterruptStackFrame stackframe = {};

        stackframe.eflags = 0x202;
        stackframe.eip = (uintptr_t)task->entry_point;
        stackframe.ebp = 0;

        stackframe.cs = 0x08;
        stackframe.ds = 0x10;
        stackframe.es = 0x10;
        stackframe.fs = 0x10;
        stackframe.gs = 0x10;

        task_kernel_stack_push(task, &stackframe, sizeof(InterruptStackFrame));
    }
}

size_t arch_debug_write(const void *buffer, size_t size) { return com_write(COM1, buffer, size); }

TimeStamp arch_get_time() { return rtc_now(); }

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

    acpi_initialize(handover);
    //lapic_initialize();
    smbios::EntryPoint *smbios_entrypoint = smbios::find({0xF0000, 65536});

    if (smbios_entrypoint)
    {
        logger_info("Found SMBIOS entrypoint at %08x (Version %d.%02d)", smbios_entrypoint, smbios_entrypoint->major_version, smbios_entrypoint->major_version);

        smbios_entrypoint->iterate([&](smbios::Header *table) {
            logger_info(" - %s (Type=%d, StringCount=%d) ", table->name(), table->type, table->string_table_lenght());

            for (size_t i = 1; i < table->string_table_lenght(); i++)
            {
                logger_info("    - %s", table->string(i));
            }

            return Iteration::CONTINUE;
        });
    }

    system_main(handover);
}

static void reboot_8042()
{
    logger_info("Trying to reboot using the 8042...");

    uint8_t good = 0x02;
    while (good & 0x02)
        good = in8(0x64);
    out8(0x64, 0xFE);
    arch_halt();
}

__no_return void arch_reboot()
{
    early_console_enable();
    logger_warn("We don't support shutting down real hardware!");

    reboot_8042();

    logger_info("Failled to reboot: Halting!");
    system_stop();
}

static void shutdown_virtual_machines()
{
    logger_info("Maybe your are running a VM, trying to shutdown using io ports...");

    // Bochs, and older versions of QEMU(than 2.0)
    out16(0xB004, 0x2000);

    // Newer versions of QEMU
    out16(0x604, 0x2000);

    // Virtualbox
    out16(0x4004, 0x3400);
}

__no_return void arch_shutdown()
{
    early_console_enable();
    logger_warn("We don't support shutting down real hardware!");

    shutdown_virtual_machines();

    logger_error("Failled to shutdown: Halting!");
    system_stop();
}

void arch_panic_dump()
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

void arch_dump_stack_frame(void *sf)
{
    auto stackframe = reinterpret_cast<InterruptStackFrame *>(sf);

    printf("\tCS=%04x DS=%04x ES=%04x FS=%04x GS=%04x\n", stackframe->cs, stackframe->ds, stackframe->es, stackframe->fs, stackframe->gs);
    printf("\tEAX=%08x EBX=%08x ECX=%08x EDX=%08x\n", stackframe->eax, stackframe->ebx, stackframe->ecx, stackframe->edx);
    printf("\tEDI=%08x ESI=%08x EBP=%08x ESP=%08x\n", stackframe->edi, stackframe->esi, stackframe->ebp, stackframe->esp);
    printf("\tINT=%08x ERR=%08x EIP=%08x FLG=%08x\n", stackframe->intno, stackframe->err, stackframe->eip, stackframe->eflags);

    printf("\tCR0=%08x CR2=%08x CR3=%08x CR4=%08x\n", CR0(), CR2(), CR3(), CR4());

    printf("\n\tBacktrace:\n");
    backtrace_internal(stackframe->ebp);
}

void arch_backtrace()
{
    backtrace_internal(EBP());
}
