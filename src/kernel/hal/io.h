#pragma once

#include <karm-base/range.h>

namespace Hal {

struct PortRange : public USizeRange {
    using USizeRange::USizeRange;
};

struct DmaRange : public USizeRange {
    using USizeRange::USizeRange;
};

template <typename T>
static T dmaIn(size_t addr) {
    return *((volatile T *)(addr));
}

template <typename T>
static void dmaOut(size_t addr, T value) {
    *((volatile T *)(addr)) = value;
}

template <typename T>
static T portIn(uint16_t addr) {
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
        asm volatile("inq %1, %0"
                     : "=a"(value)
                     : "d"(addr));
    } else {
        static_assert(sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4);
    }
    return value;
}

template <typename T>
static void portOut(uint16_t addr, T value) {
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
    }
}

struct Io {
    enum {
        PORT,
        DMA
    } _type;

    size_t _start;
    size_t _size;

    static Io port(PortRange range) {
        return {PORT, range.start(), range.size()};
    }

    static Io port(size_t start, size_t size) {
        return {PORT, start, size};
    }

    static Io dma(DmaRange range) {
        return {DMA, range.start(), range.size()};
    }

    static Io dma(size_t start, size_t size) {
        return {DMA, start, size};
    }

    template <typename T>
    T read(size_t offset) {
        if (_type == PORT) {
            return portIn<T>(_start + offset);
        } else {
            return dmaIn<T>(_start + offset);
        }
    }

    template <typename T>
    void write(size_t offset, T value) {
        if (_type == PORT) {
            portOut<T>(_start + offset, value);
        } else {
            dmaOut(_start + offset, value);
        }
    }

    uint8_t read8(size_t offset) {
        return read<uint8_t>(offset);
    }

    void write8(size_t offset, uint8_t value) {
        write<uint8_t>(offset, value);
    }

    uint16_t read16(size_t offset) {
        return read<uint16_t>(offset);
    }

    void write16(size_t offset, uint16_t value) {
        write<uint16_t>(offset, value);
    }

    uint32_t read32(size_t offset) {
        return read<uint32_t>(offset);
    }

    void write32(size_t offset, uint32_t value) {
        write<uint32_t>(offset, value);
    }

    uint64_t read64(size_t offset) {
        return read<uint64_t>(offset);
    }

    void write64(size_t offset, uint64_t value) {
        write<uint64_t>(offset, value);
    }
};

} // namespace Hal
