#include "arch/VirtualMemory.h"

void *arch_kernel_address_space()
{
    ASSERT_NOT_REACHED();
}

bool arch_virtual_present(void *address_space, uintptr_t virtual_address)
{
    __unused(address_space);
    __unused(virtual_address);

    ASSERT_NOT_REACHED();
}

uintptr_t arch_virtual_to_physical(void *address_space, uintptr_t virtual_address)
{
    __unused(address_space);
    __unused(virtual_address);

    ASSERT_NOT_REACHED();
}

Result arch_virtual_map(void *address_space, MemoryRange physical_range, uintptr_t virtual_address, MemoryFlags flags)
{
    __unused(address_space);
    __unused(physical_range);
    __unused(virtual_address);
    __unused(flags);

    ASSERT_NOT_REACHED();
}

MemoryRange arch_virtual_alloc(void *address_space, MemoryRange physical_range, MemoryFlags flags)
{
    __unused(address_space);
    __unused(physical_range);
    __unused(flags);

    ASSERT_NOT_REACHED();
}

void arch_virtual_free(void *address_space, MemoryRange virtual_range)
{
    __unused(address_space);
    __unused(virtual_range);

    ASSERT_NOT_REACHED();
}
