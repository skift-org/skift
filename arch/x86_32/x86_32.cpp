#include <libsystem/core/Plugs.h>

#include "arch/x86/COM.h"
#include "arch/x86_32/ACPI.h"
#include "arch/x86_32/FPU.h"
#include "arch/x86_32/GDT.h"
#include "arch/x86_32/IDT.h"
#include "arch/x86_32/LAPIC.h"
#include "arch/x86_32/PIC.h"
#include "arch/x86_32/PIT.h"
#include "arch/x86_32/RTC.h"
#include "arch/x86_32/x86_32.h"

#include "kernel/firmware/SMBIOS.h"
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

size_t arch_debug_write(const void *buffer, size_t size) { return com_write(COM1, buffer, size); }

TimeStamp arch_get_time() { return rtc_now(); }

extern "C" void arch_main(void *info, uint32_t magic)
{
    __plug_init();

    com_initialize(COM1);
    com_initialize(COM2);
    com_initialize(COM3);
    com_initialize(COM4);

    auto multiboot = multiboot_initialize(info, magic);

    if (multiboot->memory_usable < 127 * 1024)
    {
        system_panic("No enoughs memory (%uKio)!", multiboot->memory_usable / 1024);
    }

    gdt_initialize();
    idt_initialize();
    pic_initialize();
    fpu_initialize();
    pit_initialize(1000);

    acpi_initialize(multiboot);
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

    system_main(multiboot);
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

    // Virtualbox, you can do shutdown
    out16(0x4004, 0x3400);
}

__no_return void arch_shutdown()
{
    shutdown_virtual_machines();

    logger_error("Failled to shutdown: Halting!");
    system_stop();
}
