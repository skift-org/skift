#pragma once

#include <karm-base/array.h>

#include "karm-base/string.h"
#include "karm-meta/traits.h"

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
    static constexpr Str _sdth_signature = "RSDT";

    u32 children[];
};

struct [[gnu::packed]] Mcfg : public Sdth {
    static constexpr Str _sdth_signature = "MCFG";

    
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

    static constexpr Str _sdth_signature = "HPET";
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

template <typename T>
concept SdthEntry =
    requires {
        { T::_sdth_signature } -> Meta::Convertible<const Str>;
    } and Meta::Convertible<T, Sdth>;

static_assert(SdthEntry<Hpet>);
static_assert(SdthEntry<Mcfg>);
static_assert(SdthEntry<Rsdt>);

} // namespace Acpi
