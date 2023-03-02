#pragma once

#include <karm-base/array.h>

namespace Crc {

using Table32 = Array<u32, 256>;

static constexpr Table32 makeTable32(u32 polynomial = 0xEDB88320) {
    Table32 table;
    for (u32 i = 0; i < table.len(); ++i) {
        u32 crc = i;
        for (u32 j = 0; j < 8; ++j) {
            if (crc & 1) {
                crc = (crc >> 1) ^ polynomial;
            } else {
                crc >>= 1;
            }
        }
        table[i] = crc;
    }
    return table;
}

static constexpr Table32 TABLE32 = makeTable32();

struct CRC {
    u32 _value = 0xFFFFFFFF;

    void sum(Bytes bytes) {
        for (u8 byte : bytes) {
            _value = TABLE32[(_value ^ byte) & 0xFF] ^ (_value >> 8);
        }
    }

    u32 value() const {
        return _value ^ 0xFFFFFFFF;
    }
};

} // namespace Crc
