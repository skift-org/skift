#include <libsystem/core/Plugs.h>

#include "archs/x86/COM.h"
#include "archs/x86/FPU.h"
#include "archs/x86/PIC.h"
#include "archs/x86/PIT.h"
#include "archs/x86_32/GDT.h"
#include "archs/x86_32/IDT.h"

#include "system/graphics/Graphics.h"

#include "acpi/ACPI.h"
#include "smbios/SMBIOS.h"

namespace Arch::x86_32
{

extern "C" void arch_x86_32_main(void *info, uint32_t magic)
{
    com_initialize(COM1);
    com_initialize(COM2);
    com_initialize(COM3);
    com_initialize(COM4);

    auto handover = handover_initialize(info, magic);

    graphic_early_initialize(handover);

    if (handover->memory_usable < 127 * 1024)
    {
        system_panic("No enough memory (%uKio)!", handover->memory_usable / 1024);
    }

    gdt_initialize();
    idt_initialize();
    pic_initialize();
    fpu_initialize();
    pit_initialize(1000);

    Acpi::initialize(handover);
    //lapic_initialize();
    Smbios::EntryPoint *smbios_entrypoint = Smbios::find({0xF0000, 0xFFFF});

    if (smbios_entrypoint)
    {
        Kernel::logln("Found SMBIOS entrypoint at {08x} (Version {}.{02d})", smbios_entrypoint, smbios_entrypoint->major_version, smbios_entrypoint->major_version);

        smbios_entrypoint->iterate([&](Smbios::Header *table) {
            Kernel::logln(" - {} (Type={}, StringCount={}) ", table->name(), (int)table->type, table->string_table_lenght());

            for (size_t i = 1; i < table->string_table_lenght(); i++)
            {
                Kernel::logln("    - {}", table->string(i));
            }

            return Iteration::CONTINUE;
        });
    }

    system_main(handover);
}

} // namespace Arch::x86_32
