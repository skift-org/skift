#pragma once

#include "kernel/memory/Memory.h"
#include "kernel/memory/Virtual.h"

template <typename T>
class MemoryWindow
{
private:
    MemoryRange _virtual_ranger;
    T *_virtual_address;

    __noncopyable(MemoryWindow);

public:
    MemoryWindow(T *physical_pointer) : MemoryWindow(reinterpret_cast<uintptr_t>(physical_pointer), sizeof(T)) {}

    MemoryWindow(uintptr_t address, size_t size)
    {
        size_t offset_withing_page = address % ARCH_PAGE_SIZE;

        _virtual_ranger = virtual_alloc(memory_kpdir(), memory_range_around_non_aligned_address(address, size + offset_withing_page), MEMORY_NONE);
        _virtual_address = reinterpret_cast<T *>(_virtual_ranger.base + offset_withing_page);
    }

    ~MemoryWindow()
    {
        virtual_free(memory_kpdir(), _virtual_ranger);
    }

    T *operator->()
    {
        return _virtual_address;
    }
};
