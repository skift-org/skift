#pragma once

#include <karm-base/array.h>

namespace Crc {

using Table32 = Array<uint32_t, 256>;

static constexpr Table32 makeTable32(uint32_t polynomial = 0xEDB88320) {
    Table32 table;
    for (uint32_t i = 0; i < table.len(); ++i) {
        uint32_t crc = i;
        for (uint32_t j = 0; j < 8; ++j) {
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
    uint32_t _value = 0xFFFFFFFF;

    void sum(Bytes bytes) {
        for (uint8_t byte : bytes) {
            _value = TABLE32[(_value ^ byte) & 0xFF] ^ (_value >> 8);
        }
    }

    uint32_t value() const {
        return _value ^ 0xFFFFFFFF;
    }
};

} // namespace Crc
