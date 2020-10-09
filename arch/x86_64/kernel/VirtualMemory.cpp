#include "arch/VirtualMemory.h"

void *arch_kernel_address_space()
{
    ASSERT_NOT_REACHED();
}

void arch_virtual_initialize()
{
    ASSERT_NOT_REACHED();
}

void arch_virtual_memory_enable()
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

void *arch_address_space_create()
{
    ASSERT_NOT_REACHED();
}

void arch_address_space_destroy(void *address_space)
{
    __unused(address_space);

    ASSERT_NOT_REACHED();
}

void arch_address_space_switch(void *address_space)
{
    __unused(address_space);

    ASSERT_NOT_REACHED();
}

void arch_address_space_dump(void *address_space, bool user)
{
    __unused(address_space);
    __unused(user);

    ASSERT_NOT_REACHED();
}
