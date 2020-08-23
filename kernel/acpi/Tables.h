#pragma once

#include <libsystem/Common.h>
#include <libsystem/Logger.h>

struct __packed RSDP
{
    char signature[8];
    uint8_t checksum;
    char OEM_id[6];
    uint8_t revision;
    uint32_t rsdt_address;
};

struct __packed SDTH
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

struct __packed RSDT
{
    SDTH header;
    uint32_t childs[];

    SDTH *child(size_t index)
    {
        return reinterpret_cast<SDTH *>(childs[index]);
    }

    size_t child_count()
    {
        return (header.Length - sizeof(header)) / 4;
    }
};
