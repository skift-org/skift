#pragma once

import Karm.Core;

namespace Acpi {

struct [[gnu::packed]] Rsdp {
    Array<char, 8> signature;
    u8 checksum;
    Array<char, 6> oemId;
    u8 revision;
    u32 rsdt;
};

struct [[gnu::packed]] Sdth {
    Array<char, 4> signature;
    u32 len;
    u8 revision;
    u8 checksum;
    Array<char, 6> oemId;
    Array<char, 8> oemTableId;
    u32 oemRevision;
    u32 creatorId;
    u32 creatorRevision;
};

struct [[gnu::packed]] Rsdt : public Sdth {
    u32 children[];
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
        u8 type;
        u8 len;
    };

    struct [[gnu::packed]] LapicRecord : public Record {
        u8 processorId;
        u8 id;
        u32 flags;
    };

    struct [[gnu::packed]] IoapicRecord : public Record {
        u8 id;
        u8 reserved;
        u32 address;
        u32 interruptBase;
    };

    struct [[gnu::packed]] IsoRecord : public Record {
        u8 bus;
        u8 irq;
        u32 gsi;
        u16 flags;
    };

    u32 lapic;
    u32 flags;

    Record records[];
};

struct [[gnu::packed]] Mcfg : public Sdth {
    struct Record {
        u64 address;
        u16 segment_groupe;
        u8 busStart;
        u8 busEnd;
        u32 reserved;
    };

    u64 reserved;
    Record records[];
};

struct [[gnu::packed]] Hpet : public Sdth {
    u8 hardwareRevId;
    u8 info;
    u16 pciVendorId;
    u8 addressSpaceId;
    u8 registerBitWidth;
    u8 registerBitOffset;
    u8 reserved1;
    u64 address;
    u8 hpetNumber;
    u16 minimumTick;
    u8 pageProtection;
};

} // namespace Acpi
