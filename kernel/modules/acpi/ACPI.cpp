#include "system/Streams.h"

#include "archs/x86/IOAPIC.h"
#include "archs/x86/LAPIC.h"

#include "acpi/ACPI.h"

namespace Acpi
{

void madt_initialize(MADT *madt)
{
    Kernel::logln("MADT found, size is {}", madt->record_count());

    Arch::x86::lapic_found(madt->local_apic);

    madt->foreach_record([](auto record) {
        switch (record->type)
        {
        case MADTRecordType::LAPIC:
        {
            auto local_apic = reinterpret_cast<MADTLocalApicRecord *>(record);
            Kernel::logln("Local APIC (cpu_id={}, apic_id={}, flags={08x})", local_apic->processor_id, local_apic->apic_id, local_apic->flags);
        }
        break;

        case MADTRecordType::IOAPIC:
        {
            auto ioapic = reinterpret_cast<MADTIOApicRecord *>(record);
            Kernel::logln("I/O APIC (id={d}, address={08x})", ioapic->id, ioapic->address);
            Arch::x86::ioapic_found(ioapic->address);
        }
        break;

        case MADTRecordType::ISO:
            Kernel::logln("Interrupt Source Override");
            break;

        case MADTRecordType::NMI:
            Kernel::logln("Non-maskable interrupts");
            break;

        case MADTRecordType::LAPIC_OVERRIDE:
            Kernel::logln("Local APIC Address Override");
            break;

        default:
            break;
        }

        return Iteration::CONTINUE;
    });
}

void initialize(Handover *handover)
{
    if (!handover->acpi_rsdp_address)
    {
        Kernel::logln("No acpi rsdp found!");
        return;
    }

    RSDP *rsdp = (RSDP *)(handover->acpi_rsdp_address);

    RSDT *rsdt = (RSDT *)((uintptr_t)rsdp->rsdt_address);

    MADT *madt = (MADT *)rsdt->child("APIC");

    madt_initialize(madt);
}

} // namespace Acpi
