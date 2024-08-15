#pragma once

// https://github.com/Michaelangel007/crc32

#include <karm-base/array.h>
#include <karm-base/slice.h>

namespace Karm::Crypto {

static Array<u32, 16> const CRC32_TAB = {
    0x00000000, 0x1DB71064, 0x3B6E20C8, 0x26D930AC, 0x76DC4190,
    0x6B6B51F4, 0x4DB26158, 0x5005713C, 0xEDB88320, 0xF00F9344,
    0xD6D6A3E8, 0xCB61B38C, 0x9B64C2B0, 0x86D3D2D4, 0xA00AE278,
    0xBDBDF21C
};

struct Crc32 {
    using Digest = u32;

    u32 _crc = 0xFFFFFFFF;

    always_inline Crc32() = default;

    always_inline Crc32(u32 crc) : _crc(crc) {}

    always_inline void update(u8 byte) {
        _crc ^= byte;
        _crc = CRC32_TAB[_crc & 0x0F] ^ (_crc >> 4);
        _crc = CRC32_TAB[_crc & 0x0F] ^ (_crc >> 4);
    }

    always_inline void update(Bytes bytes) {
        auto [buf, len] = bytes;

        for (usize i = 0; i < len; ++i)
            update(buf[i]);
    }

    always_inline Digest digest() {
        return _crc ^ 0xFFFFFFFF;
    }
};

static inline Crc32::Digest crc32(Bytes bytes, u32 crc = 0xFFFFFFFF) {
    Crc32 crc32{crc};
    crc32.update(bytes);
    return crc32.digest();
}

} // namespace Karm::Crypto
