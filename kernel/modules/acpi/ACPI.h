#pragma once

#include <string.h>

#include <libutils/Iteration.h>

#include "system/handover/Handover.h"

namespace Acpi
{

/* --- RSDP ----------------------------------------------------------------- */

struct PACKED RSDP
{
    char signature[8];
    uint8_t checksum;
    char OEM_id[6];
    uint8_t revision;
    uint32_t rsdt_address;
};

/* --- SDTH ----------------------------------------------------------------- */

struct PACKED SDTH
{
    char Signature[4];
    uint32_t Length;
    uint8_t Revision;
    uint8_t Checksum;
    char OEMID[6];
    char OEMTableID[8];
    uint32_t OEMRevision;
    uint32_t CreatorID;
    uint32_t CreatorRevision;
};

/* --- RSDT ----------------------------------------------------------------- */

struct PACKED RSDT
{
    SDTH header;
    uint32_t children[];

    SDTH *child(size_t index)
    {
        return reinterpret_cast<SDTH *>(children[index]);
    }

    size_t child_count()
    {
        return (header.Length - sizeof(header)) / 4;
    }

    SDTH *child(const char *signature)
    {
        for (size_t i = 0; i < child_count(); i++)
        {
            SDTH *sdth = child(i);

            if (memcmp(sdth->Signature, signature, 4) == 0)
            {
                return sdth;
            }
        }

        return nullptr;
    }
};

/* --- MADT ----------------------------------------------------------------- */

enum struct MADTRecordType : uint8_t
{
    LAPIC = 0,
    IOAPIC = 1,
    ISO = 2,
    NMI = 4,
    LAPIC_OVERRIDE = 5,
};

struct PACKED MADTRecord
{
    MADTRecordType type;
    uint8_t lenght;
};

struct PACKED MADTLocalApicRecord
{
    MADTRecord header;
    uint8_t processor_id;
    uint8_t apic_id;
    uint32_t flags;
};

struct PACKED MADTIOApicRecord
{
    MADTRecord header;
    uint8_t id;
    uint8_t reserved;
    uint32_t address;
    uint32_t interrupt_base;
};

struct PACKED MADT
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

void initialize(Handover *handover);

} // namespace Acpi
