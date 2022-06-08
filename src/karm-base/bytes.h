#pragma once

#include <karm-base/slice.h>

namespace Karm {

enum struct Byte : uint8_t {};

struct Bytes : Slice<Byte> {
    using Slice<Byte>::Slice;

    Bytes(void const *buf, size_t len) : Slice<Byte>((Byte *)buf, len) {}
};

struct MutBytes : MutSlice<Byte> {
    using MutSlice<Byte>::MutSlice;

    MutBytes(void *buf, size_t len) : MutSlice<Byte>((Byte *)buf, len) {}
};

} // namespace Karm
