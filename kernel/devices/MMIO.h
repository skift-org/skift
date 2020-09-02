#pragma once

#include <libsystem/Common.h>

static inline uint8_t mmio_read8(uint64_t address)
{
    return *((volatile uint8_t *)address);
}

static inline uint16_t mmio_read16(uint64_t address)
{
    return *((volatile uint16_t *)address);
}

static inline uint32_t mmio_read32(uint64_t address)
{
    return *((volatile uint32_t *)address);
}

static inline uint64_t mmio_read64(uint64_t address)
{
    return *((volatile uint64_t *)address);
}

static inline void mmio_write8(uint64_t address, uint8_t value)
{
    (*((volatile uint8_t *)address)) = value;
}

static inline void mmio_write16(uint64_t address, uint16_t value)
{
    (*((volatile uint16_t *)address)) = value;
}

static inline void mmio_write32(uint64_t address, uint32_t value)
{
    (*((volatile uint32_t *)address)) = value;
}

static inline void mmio_write64(uint64_t address, uint64_t value)
{
    (*((volatile uint64_t *)address)) = value;
}
