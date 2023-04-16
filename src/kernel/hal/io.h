#pragma once

#include <karm-base/range.h>

namespace Hal {

using PortRange = Range<usize, struct PortRangeTag>;

struct DmaRange : public USizeRange {
    using USizeRange::USizeRange;
};

template <typename T>
static T dmaIn(usize addr) {
    return *((T volatile *)(addr));
}

template <typename T>
static void dmaOut(usize addr, T value) {
    *((T volatile *)(addr)) = value;
}

template <typename T>
static T portIn(u16 addr) {
    T value = 0;
    if constexpr (sizeof(T) == 1) {
        asm volatile("inb %1, %0"
                     : "=a"(value)
                     : "d"(addr));
    } else if constexpr (sizeof(T) == 2) {
        asm volatile("inw %1, %0"
                     : "=a"(value)
                     : "d"(addr));
    } else if constexpr (sizeof(T) == 4) {
        asm volatile("inl %1, %0"
                     : "=a"(value)
                     : "d"(addr));
    } else if constexpr ((sizeof(T) == 8)) {
        // x86_64 doesn't support 64bit I/O operation so we emulate it
        u64 res = portIn<u32>(addr);
        res |= (u64)portIn<u32>(addr + 4) << 32;
        return res;
    } else {
        static_assert(sizeof(T) == 1 or sizeof(T) == 2 or sizeof(T) == 4);
    }
    return value;
}

template <typename T>
static void portOut(u16 addr, T value) {
    if constexpr (sizeof(T) == 1) {
        asm volatile("outb %0, %1"
                     :
                     : "a"(value), "d"(addr));
    } else if constexpr (sizeof(T) == 2) {
        asm volatile("outw %0, %1"
                     :
                     : "a"(value), "d"(addr));
    } else if constexpr (sizeof(T) == 4) {
        asm volatile("outl %0, %1"
                     :
                     : "a"(value), "d"(addr));
    } else if constexpr ((sizeof(T) == 8)) {
        // x86_64 doesn't support 64bit I/O operation so we emulate it
        portOut<u32>(addr, value);
        portOut<u32>(addr + 4, value >> 32);
    } else {
        static_assert(sizeof(T) == 1 or sizeof(T) == 2 or sizeof(T) == 4);
    }
}

struct Io {
    enum Type {
        PORT,
        DMA
    } _type;

    usize _start;
    usize _size;

    static Io port(PortRange range) {
        return {PORT, range.start, range.size};
    }

    static Io port(usize start, usize size) {
        return {PORT, start, size};
    }

    static Io dma(DmaRange range) {
        return {DMA, range.start, range.size};
    }

    static Io dma(usize start, usize size) {
        return {DMA, start, size};
    }

    template <typename T>
    T read(usize offset) {
        if (_type == PORT) {
            return portIn<T>(_start + offset);
        } else {
            return dmaIn<T>(_start + offset);
        }
    }

    template <typename T>
    void write(usize offset, T value) {
        if (_type == PORT) {
            portOut<T>(_start + offset, value);
        } else {
            dmaOut(_start + offset, value);
        }
    }

    usize readSized(usize offset, usize size) {
        if (size == 1) {
            return read8(offset);
        } else if (size == 2) {
            return read16(offset);
        } else if (size == 4) {
            return read32(offset);
        } else if (size == 8) {
            return read64(offset);
        } else {
            return 0;
        }
    }

    void writeSized(usize offset, usize size, usize value) {
        if (size == 1) {
            write8(offset, value);
        } else if (size == 2) {
            write16(offset, value);
        } else if (size == 4) {
            write32(offset, value);
        } else if (size == 8) {
            write64(offset, value);
        }
    }

    u8 read8(usize offset) {
        return read<u8>(offset);
    }

    void write8(usize offset, u8 value) {
        write<u8>(offset, value);
    }

    u16 read16(usize offset) {
        return read<u16>(offset);
    }

    void write16(usize offset, u16 value) {
        write<u16>(offset, value);
    }

    u32 read32(usize offset) {
        return read<u32>(offset);
    }

    void write32(usize offset, u32 value) {
        write<u32>(offset, value);
    }

    u64 read64(usize offset) {
        return read<u64>(offset);
    }

    void write64(usize offset, u64 value) {
        write<u64>(offset, value);
    }
};

} // namespace Hal
