#pragma once

#include <karm-base/res.h>

#include "../bscan.h"

namespace DeviceTree {

struct Blob {
    struct Header {
        be_uint32_t magic;
        be_uint32_t totalsize;
        be_uint32_t offDtStruct;
        be_uint32_t offDtStrings;
        be_uint32_t offMemRsvmap;
        be_uint32_t version;
        be_uint32_t lastCompVersion;
        be_uint32_t bootCpuidPhys;
        be_uint32_t sizeDtStrings;
        be_uint32_t sizeDtStruct;
    };

    struct Reserved {
        uintptr_t address;
        size_t size;
    };

    static constexpr uint32_t magic = 0xd00dfeed;

    Bytes _slice;

    static Res<Blob> load(void *ptr) {
        BScan scan{Bytes{static_cast<Byte *>(ptr), sizeof(Header)}};
        auto magic = scan.nextBeUint32();

        if (magic != Blob::magic) {
            return Error{"invalid magic"};
        }

        auto size = scan.nextBeUint32();

        if (size < sizeof(Header)) {
            return Error{"invalid size"};
        }

        return Blob{Bytes{static_cast<Byte *>(ptr), size}};
    }

    BScan begin() const { return _slice; }

    Header header() const {
        Header h{};
        begin().peekTo(&h, sizeof(h));
        return h;
    }

    bool valid() const {
        auto h = header();
        return h.magic == magic;
    }

    auto iterReserved() const {
        auto s = begin();
        s.skip(header().offMemRsvmap);
        return Iter{
            [s]() mutable -> Opt<Reserved> {
                if (s.ended()) {
                    return NONE;
                }

                auto addr = s.nextBeUint64();
                auto size = s.nextBeUint64();

                if (addr == 0 and size == 0) {
                    return NONE;
                }

                return Reserved{addr, size};
            }};
    }
};

} // namespace DeviceTree
