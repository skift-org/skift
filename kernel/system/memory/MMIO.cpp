#include "system/Streams.h"

#include "archs/Arch.h"

#include "system/interrupts/Interupts.h"
#include "system/memory/MMIO.h"
#include "system/memory/Physical.h"

MMIORange::MMIORange()
{
}

MMIORange::MMIORange(size_t size)
{
    size = PAGE_ALIGN_UP(size);

    InterruptsRetainer retainer;

    _own_physical_range = true;
    _physical_range = {physical_alloc(size)};
    _virtual_range = {Arch::virtual_alloc(Arch::kernel_address_space(), _physical_range, MEMORY_NONE)};
}

MMIORange::MMIORange(MemoryRange range)
{
    InterruptsRetainer retainer;

    _physical_range = {range};
    _virtual_range = {Arch::virtual_alloc(Arch::kernel_address_space(), _physical_range, MEMORY_NONE)};

    Kernel::logln("Created MMIO region {08x}-{08x} mapped to {08x}-{08x}",
                  range.base(), range.end(), _virtual_range.base(), _virtual_range.end());
}

MMIORange::~MMIORange()
{
    if (empty())
        return;

    InterruptsRetainer retainer;

    Arch::virtual_free(Arch::kernel_address_space(), _virtual_range);

    if (!_own_physical_range)
        return;

    physical_free(_physical_range);
}
