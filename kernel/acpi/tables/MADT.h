#pragma once

#include "kernel/acpi/tables/SDTH.h"

struct __packed MADTRecord
{
#define MADTH_LAPIC (0)
#define MADTH_IOAPIC (1)
#define MADTH_ISO (2)
#define MADTH_NMI (4)
#define MADTH_LAPIC_OVERRIDE (5)
    uint8_t type;
    uint8_t lenght;
};

struct __packed MADTLocalApicRecord
{
    MADTRecord header;
    uint8_t processor_id;
    uint8_t apic_id;
    uint32_t flags;
};

struct __packed MADTIOApicRecord
{
    MADTRecord header;
    uint8_t id;
    uint8_t reserved;
    uint32_t address;
    uint32_t interrupt_base;
};

struct __packed MADT
{
    SDTH header;

    uint32_t local_apic;
    uint32_t flags;

    MADTRecord records[];

    template <typename Callback>
    void foreach_record(Callback callback)
    {
        MADTRecord *current = records;

        while ((uintptr_t)current < (uintptr_t)&header + header.Length)
        {
            current = (MADTRecord *)(((uintptr_t)current) + current->lenght);

            if (callback(current) != Iteration::CONTINUE)
            {
                return;
            }
        }
    }

    size_t record_count()
    {
        size_t result = 0;
        foreach_record([&](auto) { result++;  return Iteration::CONTINUE; });
        return result;
    }
};
