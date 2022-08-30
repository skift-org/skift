#pragma once

#include <karm-base/array.h>

namespace Acpi {

struct [[gnu::packed]] Rsdp {
    Array<char, 8> signature;
    uint8_t checksum;
    Array<char, 6> oemId;
    uint8_t revision;
    uint32_t rsdt;
};

struct [[gnu::packed]] Sdth {
    Array<char, 4> signature;
    uint32_t len;
    uint8_t revision;
    uint8_t checksum;
    Array<char, 6> oemId;
    Array<char, 8> oemTableId;
    uint32_t oemRevision;
    uint32_t creatorId;
    uint32_t creatorRevision;
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
        uint8_t processorId;
        uint8_t id;
        uint32_t flags;
    };

    struct [[gnu::packed]] IoapicRecord : public Record {
        uint8_t id;
        uint8_t reserved;
        uint32_t address;
        uint32_t interruptBase;
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
        uint8_t busStart;
        uint8_t busEnd;
        uint32_t reserved;
    };

    uint64_t reserved;
    Record records[];
};

struct [[gnu::packed]] Hpet : public Sdth {
    uint8_t hardwareRevId;
    uint8_t info;
    uint16_t pciVendorId;
    uint8_t addressSpaceId;
    uint8_t registerBitWidth;
    uint8_t registerBitOffset;
    uint8_t reserved1;
    uint64_t address;
    uint8_t hpetNumber;
    uint16_t minimumTick;
    uint8_t pageProtection;
};

} // namespace Acpi
