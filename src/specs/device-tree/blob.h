#pragma once

import Karm.Core;

using namespace Karm;

namespace DeviceTree {

struct Blob {
    struct Header {
        u32be magic;
        u32be totalsize;
        u32be offDtStruct;
        u32be offDtStrings;
        u32be offMemRsvmap;
        u32be version;
        u32be lastCompVersion;
        u32be bootCpuidPhys;
        u32be sizeDtStrings;
        u32be sizeDtStruct;
    };

    struct Reserved {
        u64 address;
        u64 size;
    };

    static constexpr u32 MAGIC = 0xd00dfeed;

    Bytes _slice;

    static Res<Blob> load(Bytes bytes) {
        Io::BScan scan{bytes};
        auto magic = scan.nextU32be();

        if (magic != MAGIC) {
            return Error::invalidData("invalid magic");
        }

        auto size = scan.nextU32be();

        if (size < sizeof(Header)) {
            return Error::invalidData("invalid size");
        }

        return Ok(Blob{bytes});
    }

    Io::BScan begin() const { return _slice; }

    Header header() const {
        Header h{};
        begin().peekTo(&h);
        return h;
    }

    bool valid() const {
        auto h = header();
        return h.magic == MAGIC;
    }

    auto iterReserved() const {
        auto s = begin();
        s.skip(header().offMemRsvmap);
        return Iter{
            [s] mutable -> Opt<Range<u64>> {
                if (s.ended()) {
                    return NONE;
                }

                auto addr = s.nextU64be();
                auto size = s.nextU64be();

                if (addr == 0 and size == 0) {
                    return NONE;
                }

                return Range<u64>{addr, size};
            },
        };
    }
};

} // namespace DeviceTree
