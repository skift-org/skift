#include <libsystem/Logger.h>

#include "arch/x86/ACPI.h"
#include "arch/x86/IOAPIC.h"
#include "arch/x86/LAPIC.h"
#include "kernel/acpi/tables/MADT.h"
#include "kernel/acpi/tables/RSDP.h"
#include "kernel/acpi/tables/RSDT.h"

void acpi_madt_initialize(MADT *madt)
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
    RSDP *rsdp = (RSDP *)(multiboot->acpi_rsdp_address);

    RSDT *rsdt = (RSDT *)(rsdp->rsdt_address);

    MADT *madt = (MADT *)rsdt->child("APIC");

    acpi_madt_initialize(madt);
}
