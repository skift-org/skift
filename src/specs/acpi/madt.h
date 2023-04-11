#pragma once
#include <acpi/spec.h>
#include <karm-base/iter.h>
#include <karm-base/res.h>
#include "hal-x86_64/apic.h"

namespace Acpi {

template <typename T>
concept MadtRecord =
    requires (T const &t){
        { T::_rec_id } -> Meta::Convertible<u8>;
    };

struct [[gnu::packed]] Madt : public Sdth {

    static constexpr Str _sdth_signature = "APIC";

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
        static constexpr u8 _rec_id = 0;
        u8 processorId;
        u8 id;
        u32 flags;
    };

    struct [[gnu::packed]] IoapicRecord : public Record {
        static constexpr u8 _rec_id = 1;
        u8 id;
        u8 reserved;
        u32 address;
        u32 interruptBase;
    };

    struct [[gnu::packed]] IsoRecord : public Record {
        static constexpr u8 _rec_id = 2;
        u8 bus;
        u8 irq;
        u32 gsi;
        u16 flags;
    };

    u32 lapic;
    u32 flags;

    Record records[];

    auto iterRecords() const {

        Record const * records = this->records;
        Record const * next = records;
        Record const * end = reinterpret_cast<Record const *>(reinterpret_cast<usize>(this) + this->len);

        return Iter{[next, end]() mutable -> Opt<Record const *> {
            
            Record const * cur = next;
            
            usize cnext = reinterpret_cast<usize>(cur) + cur->len;
            
            next = reinterpret_cast<Record const *>(cnext);
            if (cur >= end) {
                return NONE;
            }
            return cur;
        }};

    }

    template <MadtRecord T>
    Opt<T const *> getFirstRecord() const {
        auto r = (iterRecords().find([](auto const &record) {
            return (*record).type == T::_rec_id;
        }));

        if(!r) {
            return NONE;
        }

        return reinterpret_cast<T const *>(r.unwrap());
    }


    void forEach(auto fn) const  {
        for (auto const r : iterRecords()) {
            fn(r);
        }
    }

    template<MadtRecord T>
    void forEach(auto fn) const  {
        this->forEach([&](Record const *r) {
            if(r->type == T::_rec_id)
            {
                fn(reinterpret_cast<T const *>(r));
            }
        });
    }
};

static_assert(SdthEntry<Madt>);

static_assert(MadtRecord<Madt::IoapicRecord>);
static_assert(MadtRecord<Madt::IsoRecord>);
static_assert(MadtRecord<Madt::LapicRecord>);

} // namespace Acpi