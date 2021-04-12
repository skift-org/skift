#include <libsystem/Logger.h>

#include "archs/Arch.h"

#include "archs/x86/IOPort.h"
#include "archs/x86/x86.h"
#include "archs/x86_32/ACPI.h"
#include "archs/x86_32/Power.h"

namespace x86
{

void reboot_8042()
{
    logger_info("Trying to reboot using the 8042...");

    uint8_t good = 0x02;

    while (good & 0x02)
    {
        good = in8(0x64);
    }

    out8(0x64, 0xFE);
    arch_halt();
}

void reboot_triple_fault()
{
    logger_info("Trying to reboot by doing a triple fault...");
    cli();
    arch_address_space_switch(0x0);
    *(uint32_t *)0x0 = 0xDEADDEAD;
}

void shutdown_virtual_machines()
{
    logger_info("Maybe your are running a VM, trying to shutdown using io ports...");

    // Bochs, and older versions of QEMU(than 2.0)
    out16(0xB004, 0x2000);

    // Newer versions of QEMU
    out16(0x604, 0x2000);

    // Virtualbox
    out16(0x4004, 0x3400);
}

void shutdown_acpi()
{
    logger_info("Trying to shutdown using acpi...");
}

} // namespace x86