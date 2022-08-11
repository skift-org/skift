#pragma once

#include <karm-base/array.h>

namespace Acpi {

struct [[gnu::packed]] Rsdp {
    Array<char, 8> signature;
    uint8_t checksum;
    Array<char, 6> OEM_id;
    uint8_t revision;
    uint32_t rsdt;
};

struct [[gnu::packed]] Sdth {
    Array<char, 4> signature;
    uint32_t len;
    uint8_t revision;
    uint8_t checksum;
    Array<char, 6> oem_id;
    Array<char, 8> oem_table_id;
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
};

struct [[gnu::packed]] Rsdt : public Sdth {
    uint32_t children[];
};

struct [[gnu::packed]] Madt : public Sdth {
    enum struct Type {
        LAPIC = 0,
        IOAPIC = 1,
        ISO = 2,
        NMI = 4,
        LAPIC_OVERRIDE = 5
    };

    struct [[gnu::packed]] Record {
        uint8_t type;
        uint8_t len;
    };

    struct [[gnu::packed]] LapicRecord : public Record {
        uint8_t processor_id;
        uint8_t id;
        uint32_t flags;
    };

    struct [[gnu::packed]] IoapicRecord : public Record {
        uint8_t id;
        uint8_t reserved;
        uint32_t address;
        uint32_t interrupt_base;
    };

    struct [[gnu::packed]] IsoRecord : public Record {
        uint8_t bus;
        uint8_t irq;
        uint32_t gsi;
        uint16_t flags;
    };

    uint32_t lapic;
    uint32_t flags;

    Record records[];
};

struct [[gnu::packed]] Mcfg : public Sdth {
    struct Record {
        uint64_t address;
        uint16_t segment_groupe;
        uint8_t bus_start;
        uint8_t bus_end;
        uint32_t reserved;
    };

    uint64_t reserved;
    Record records[];
};

struct [[gnu::packed]] Hpet : public Sdth {
    uint8_t hardware_rev_id;
    uint8_t info;
    uint16_t pci_vendor_id;
    uint8_t address_space_id;
    uint8_t register_bit_width;
    uint8_t register_bit_offset;
    uint8_t reserved1;
    uint64_t address;
    uint8_t hpet_number;
    uint16_t minimum_tick;
    uint8_t page_protection;
};

} // namespace Acpi
