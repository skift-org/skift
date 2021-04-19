#pragma once

#include <libmath/MinMax.h>
#include <libsystem/Common.h>
#include <libutils/RefCounted.h>
#include <string.h>

#include "kernel/memory/MemoryRange.h"

class MMIORange : public RefCounted<MMIORange>
{
private:
    MemoryRange _virtual_range = {};
    bool _own_physical_range = false;
    MemoryRange _physical_range = {};

public:
    uintptr_t base() { return _virtual_range.base(); }

    uintptr_t physical_base() { return _physical_range.base(); }

    uintptr_t end() { return _virtual_range.end(); }

    uintptr_t physical_end() { return _physical_range.end(); }

    size_t size() { return _virtual_range.size(); }

    bool empty() { return _virtual_range.empty(); }

    MMIORange();

    MMIORange(size_t size);

    MMIORange(MemoryRange range);

    ~MMIORange();

    size_t read(size_t offset, void *buffer, size_t size)
    {
        size_t read = 0;

        if (offset <= _virtual_range.size())
        {
            read = MIN(_virtual_range.size() - offset, size);
            memcpy(buffer, (char *)_virtual_range.base() + offset, read);
        }

        return read;
    }

    uint8_t read8(size_t offset)
    {
        return *(volatile uint8_t *)(_virtual_range.base() + offset);
    }

    uint16_t read16(size_t offset)
    {
        return *(volatile uint16_t *)(_virtual_range.base() + offset);
    }

    uint32_t read32(size_t offset)
    {
        return *(volatile uint32_t *)(_virtual_range.base() + offset);
    }

    uint64_t read64(size_t offset)
    {
        return *(volatile uint64_t *)(_virtual_range.base() + offset);
    }

    size_t write(size_t offset, const void *buffer, size_t size)
    {
        size_t written = 0;

        if (offset <= _virtual_range.size())
        {
            written = MIN(_virtual_range.size() - offset, size);
            memcpy((char *)_virtual_range.base() + offset, buffer, written);
        }

        return written;
    }

    void write8(size_t offset, uint8_t value)
    {
        (*(volatile uint8_t *)(_virtual_range.base() + offset)) = value;
    }

    void write16(size_t offset, uint16_t value)
    {
        (*(volatile uint16_t *)(_virtual_range.base() + offset)) = value;
    }

    void write32(size_t offset, uint32_t value)
    {
        (*(volatile uint32_t *)(_virtual_range.base() + offset)) = value;
    }

    void write64(size_t offset, uint64_t value)
    {
        (*(volatile uint64_t *)(_virtual_range.base() + offset)) = value;
    }
};
