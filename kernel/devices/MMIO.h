#pragma once

#include <libsystem/Common.h>

static inline uint8_t mmio_read8(uint64_t p_address)
{
    return *((volatile uint8_t *)(p_address));
}

static inline uint16_t mmio_read16(uint64_t p_address)
{
    return *((volatile uint16_t *)(p_address));
}

static inline uint32_t mmio_read32(uint64_t p_address)
{
    return *((volatile uint32_t *)(p_address));
}

static inline uint64_t mmio_read64(uint64_t p_address)
{
    return *((volatile uint64_t *)(p_address));
}

static inline void mmio_write8(uint64_t p_address, uint8_t p_value)
{
    (*((volatile uint8_t *)(p_address))) = (p_value);
}

static inline void mmio_write16(uint64_t p_address, uint16_t p_value)
{
    (*((volatile uint16_t *)(p_address))) = (p_value);
}

static inline void mmio_write32(uint64_t p_address, uint32_t p_value)
{
    (*((volatile uint32_t *)(p_address))) = (p_value);
}

static inline void mmio_write64(uint64_t p_address, uint64_t p_value)
{
    (*((volatile uint64_t *)(p_address))) = (p_value);
}
