#include <libsystem/Logger.h>

#include "architectures/VirtualMemory.h"
#include "architectures/x86_64/kernel/Paging.h"

PageMappingLevel4 kpml4 __aligned(ARCH_PAGE_SIZE) = {};
PageMappingLevel3 kpml3 __aligned(ARCH_PAGE_SIZE) = {};

PageMappingLevel2 kpml2 __aligned(ARCH_PAGE_SIZE) = {};
PageMappingLevel1 kpml1[512] __aligned(ARCH_PAGE_SIZE) = {};

#define PHYSICAL_OFFSET (0xffffffff80000000)
#define FROM_KERNEL_TO_PHYSICAL(__addr) ((uintptr_t)(__addr)-PHYSICAL_OFFSET)
#define FROM_PHYSICAL_TO_KERNEL(__addr) ((uintptr_t)(__addr) + PHYSICAL_OFFSET)

void *arch_kernel_address_space()
{
    return &kpml4;
}

void arch_virtual_initialize()
{
    logger_debug("Initializing virtual maps...");
    auto &pml4_entry = kpml4.entries[511];
    pml4_entry.user = 0;
    pml4_entry.writable = 1;
    pml4_entry.present = 1;
    pml4_entry.physical_address = FROM_KERNEL_TO_PHYSICAL(&kpml3) / ARCH_PAGE_SIZE;

    auto &pml3_entry = kpml3.entries[511];
    pml3_entry.user = 0;
    pml3_entry.writable = 1;
    pml3_entry.present = 1;
    pml3_entry.physical_address = FROM_KERNEL_TO_PHYSICAL(&kpml2) / ARCH_PAGE_SIZE;

    for (size_t i = 0; i < 512; i++)
    {
        auto &pml2_entry = kpml2.entries[i];
        pml2_entry.user = 0;
        pml2_entry.writable = 1;
        pml2_entry.present = 1;
        pml2_entry.physical_address = FROM_KERNEL_TO_PHYSICAL(&kpml1[i]) / ARCH_PAGE_SIZE;
    }

    logger_debug("It works !");
}

void arch_virtual_memory_enable()
{
    logger_debug("Enabling paging");
    arch_address_space_switch(arch_kernel_address_space());
}

bool arch_virtual_present(void *address_space, uintptr_t virtual_address)
{
    auto pml4 = reinterpret_cast<PageMappingLevel4 *>(address_space);
    auto &pml4_entry = pml4->entries[pml4_index(virtual_address)];

    if (!pml4_entry.present)
    {
        return false;
    }

    auto pml3 = reinterpret_cast<PageMappingLevel3 *>(FROM_PHYSICAL_TO_KERNEL(pml4_entry.physical_address * ARCH_PAGE_SIZE));
    auto &pml3_entry = pml3->entries[pml3_index(virtual_address)];

    if (!pml3_entry.present)
    {
        return false;
    }

    auto pml2 = reinterpret_cast<PageMappingLevel2 *>(FROM_PHYSICAL_TO_KERNEL(pml3_entry.physical_address * ARCH_PAGE_SIZE));
    auto &pml2_entry = pml2->entries[pml2_index(virtual_address)];

    if (!pml2_entry.present)
    {
        return false;
    }

    auto pml1 = reinterpret_cast<PageMappingLevel1 *>(FROM_PHYSICAL_TO_KERNEL(pml2_entry.physical_address * ARCH_PAGE_SIZE));
    auto &pml1_entry = pml1->entries[pml1_index(virtual_address)];

    return pml1_entry.present;
}

uintptr_t arch_virtual_to_physical(void *address_space, uintptr_t virtual_address)
{
    auto pml4 = reinterpret_cast<PageMappingLevel4 *>(address_space);
    auto &pml4_entry = pml4->entries[pml4_index(virtual_address)];

    if (!pml4_entry.present)
    {
        return 0;
    }

    auto pml3 = reinterpret_cast<PageMappingLevel3 *>(FROM_PHYSICAL_TO_KERNEL(pml4_entry.physical_address * ARCH_PAGE_SIZE));
    auto &pml3_entry = pml3->entries[pml3_index(virtual_address)];

    if (!pml3_entry.present)
    {
        return 0;
    }

    auto pml2 = reinterpret_cast<PageMappingLevel2 *>(FROM_PHYSICAL_TO_KERNEL(pml3_entry.physical_address * ARCH_PAGE_SIZE));
    auto &pml2_entry = pml2->entries[pml2_index(virtual_address)];

    if (!pml2_entry.present)
    {
        return 0;
    }

    auto pml1 = reinterpret_cast<PageMappingLevel1 *>(FROM_PHYSICAL_TO_KERNEL(pml2_entry.physical_address * ARCH_PAGE_SIZE));
    auto &pml1_entry = pml1->entries[pml1_index(virtual_address)];

    if (!pml1_entry.present)
    {
        return 0;
    }

    return (pml1_entry.physical_address * ARCH_PAGE_SIZE) + (virtual_address & 0xfff);
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
    paging_load_directory((uintptr_t)address_space);
}

void arch_address_space_dump(void *address_space, bool user)
{
    __unused(address_space);
    __unused(user);

    ASSERT_NOT_REACHED();
}
