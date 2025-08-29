#pragma once

import Karm.Core;

#include <karm-core/macros.h>

using namespace Karm;

namespace Hal {

using PortRange = Range<usize, struct PortRangeTag>;

using DmaRange = Range<usize, struct DmaRangeTag>;

template <typename T, usize _OFF>
struct Reg {
    using Type = T;
    static constexpr usize OFF = _OFF;
};

struct Io {
    virtual ~Io() = default;

    virtual Res<usize> in(usize offset, usize size) = 0;

    virtual Res<> out(usize offset, usize size, usize value) = 0;

    virtual Res<> read(usize offset, MutBytes buf) {
        for (usize i = 0; i < buf.len(); i++) {
            buf[i] = try$(in(offset + i, 1));
        }
        return Ok();
    }

    virtual Res<> write(usize offset, Bytes buf) {
        for (usize i = 0; i < buf.len(); i++) {
            try$(out(offset + i, 1, buf[i]));
        }
        return Ok();
    }

    template <typename R>
    Res<typename R::Type> read() {
        return Ok((typename R::Type)(try$(in(R::OFF, sizeof(typename R::Type)))));
    }

    template <typename R>
    Res<> write(typename R::Type value) {
        return out(R::OFF, sizeof(typename R::Type), (usize)value);
    }

    Res<u8> in8(usize offset) {
        return Ok((u8)try$(in(offset, 1)));
    }

    Res<> out8(usize offset, u8 value) {
        return out(offset, 1, value);
    }

    Res<u16> in16(usize offset) {
        return Ok((u16)try$(in(offset, 2)));
    }

    Res<> out16(usize offset, u16 value) {
        return out(offset, 2, value);
    }

    Res<u32> in32(usize offset) {
        return Ok((u32)try$(in(offset, 4)));
    }

    Res<> out32(usize offset, u32 value) {
        return out(offset, 4, value);
    }

    Res<u64> in64(usize offset) {
        return Ok((u64)try$(in(offset, 8)));
    }

    Res<> out64(usize offset, u64 value) {
        return out(offset, 8, value);
    }
};

} // namespace Hal
