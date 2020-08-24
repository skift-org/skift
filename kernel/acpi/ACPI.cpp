#include <libsystem/Logger.h>
#include <libutils/String.h>

#include "arch/x86/IOAPIC.h"
#include "arch/x86/LAPIC.h"

#include "kernel/acpi/ACPI.h"
#include "kernel/acpi/tables/MADT.h"
#include "kernel/acpi/tables/RSDP.h"
#include "kernel/acpi/tables/RSDT.h"
#include "kernel/memory/MemoryWindow.h"

void acpi_madt_initialize(MemoryWindow<MADT> &madt)
{
    logger_info("MADT found, size is %d", madt->record_count());

    lapic_found(madt->local_apic);

    madt->foreach_record([](auto record) {
        switch (record->type)
        {
        case MADTH_LAPIC:
        {
            auto local_apic = reinterpret_cast<MADTLocalApicRecord *>(record);
            logger_info("Local APIC (cpu_id=%d, apic_id=%d, flags=%08x)", local_apic->processor_id, local_apic->apic_id, local_apic->flags);
        }
        break;

        case MADTH_IOAPIC:
        {
            auto ioapic = reinterpret_cast<MADTIOApicRecord *>(record);
            logger_info("I/O APIC (id=%d, address=%08x)", ioapic->id, ioapic->address);
            ioapic_found(ioapic->address);
        }
        break;

        case MADTH_ISO:
            logger_info("Interrupt Source Override");
            break;

        case MADTH_NMI:
            logger_info("Non-maskable interrupts");
            break;

        case MADTH_LAPIC_OVERRIDE:
            logger_info("Local APIC Address Override");
            break;

        default:
            break;
        }

        return Iteration::CONTINUE;
    });
}

void acpi_initialize(Multiboot *multiboot)
{
    logger_info("Initializing ACPI subsystem...");

    MemoryWindow<RSDP> rsdp_window(multiboot->acpi_rsdp_address, multiboot->acpi_rsdp_size);

    logger_info("RSDP(%08x) signature: '%s'", multiboot->acpi_rsdp_address, String(rsdp_window->signature, 8).cstring());

    MemoryWindow<RSDT> rsdt_window(rsdp_window->rsdt_address, sizeof(RSDT));

    logger_info("RSDT(%08x) signature: '%s'", rsdp_window->rsdt_address, String(rsdt_window->header.Signature, 4).cstring());

    for (size_t i = 0; i < rsdt_window->child_count(); i++)
    {
        MemoryWindow<SDTH> sdth_window(rsdt_window->child(i));

        String signature(sdth_window->Signature, 4);

        logger_info("(%08x) -> '%s'", rsdt_window->child(i), signature.cstring());

        if (signature == "APIC")
        {
            MemoryWindow<MADT> madt_window((uintptr_t)rsdt_window->child(i), sdth_window->Length);

            acpi_madt_initialize(madt_window);
        }
    }
}
