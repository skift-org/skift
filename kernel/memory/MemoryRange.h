#pragma once

#include <libsystem/Assert.h>

#include "kernel/memory/Paging.h"

class MemoryRange
{
private:
    uintptr_t _base;
    size_t _size;

public:
    auto base() { return _base; }

    auto end() { return _base + _size - 1; }

    auto size() { return _size; }

    auto empty() { return size() == 0; }

    constexpr MemoryRange()
        : _base(0),
          _size(0)
    {
    }

    constexpr MemoryRange(const uintptr_t base, const size_t size)
        : _base(base),
          _size(size)
    {
    }

    auto is_page_aligned()
    {
        return IS_PAGE_ALIGN(base()) && IS_PAGE_ALIGN(size());
    }

    static inline MemoryRange from_non_aligned_address(uintptr_t base, size_t size)
    {
        size_t align = PAGE_SIZE - base % PAGE_SIZE;

        if (base % PAGE_SIZE == 0)
        {
            align = 0;
        }

        base += align;
        size -= align;

        size -= size % PAGE_SIZE;

        return (MemoryRange){base, size};
    }

    static inline MemoryRange around_non_aligned_address(uintptr_t base, size_t size)
    {
        size_t align = base % PAGE_SIZE;

        base -= align;
        size += align;

        size += PAGE_SIZE - size % PAGE_SIZE;

        return (MemoryRange){base, size};
    }
};
