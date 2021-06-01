#include <libutils/ResultOr.h>

#include "system/Streams.h"
#include "system/interrupts/Interupts.h"
#include "system/memory/Memory.h"
#include "system/system/System.h"

#include "archs/Arch.h"
#include "archs/x86_64/Paging.h"
#include "archs/x86_64/x86_64.h"

namespace Arch::x86_64
{

PML4 kpml4 ALIGNED(ARCH_PAGE_SIZE) = {};
PML3 kpml3 ALIGNED(ARCH_PAGE_SIZE) = {};
PML2 kpml2 ALIGNED(ARCH_PAGE_SIZE) = {};
PML1 kpml1[512] ALIGNED(ARCH_PAGE_SIZE) = {};

PML4 *kernel_pml4()
{
    return &kpml4;
}

void virtual_initialize()
{
    auto &pml4_entry = kpml4.entries[0];
    pml4_entry.user = 0;
    pml4_entry.writable = 1;
    pml4_entry.present = 1;
    pml4_entry.physical_address = (uint64_t)&kpml3 / ARCH_PAGE_SIZE;

    auto &pml3_entry = kpml3.entries[0];
    pml3_entry.user = 0;
    pml3_entry.writable = 1;
    pml3_entry.present = 1;
    pml3_entry.physical_address = (uint64_t)&kpml2 / ARCH_PAGE_SIZE;

    for (size_t i = 0; i < 512; i++)
    {
        auto &pml2_entry = kpml2.entries[i];
        pml2_entry.user = 0;
        pml2_entry.writable = 1;
        pml2_entry.present = 1;
        pml2_entry.physical_address = (uint64_t)&kpml1[i] / ARCH_PAGE_SIZE;
    }
}

void virtual_memory_enable()
{
    pml4_switch(kernel_pml4());
}

bool virtual_present(PML4 *pml4, uintptr_t virtual_address)
{
    ASSERT_INTERRUPTS_RETAINED();

    auto &pml4_entry = pml4->entries[pml4_index(virtual_address)];

    if (!pml4_entry.present)
    {
        return false;
    }

    auto pml3 = reinterpret_cast<PML3 *>(pml4_entry.physical_address * ARCH_PAGE_SIZE);
    auto &pml3_entry = pml3->entries[pml3_index(virtual_address)];

    if (!pml3_entry.present)
    {
        return false;
    }

    auto pml2 = reinterpret_cast<PML2 *>(pml3_entry.physical_address * ARCH_PAGE_SIZE);
    auto &pml2_entry = pml2->entries[pml2_index(virtual_address)];

    if (!pml2_entry.present)
    {
        return false;
    }

    auto pml1 = reinterpret_cast<PML1 *>(pml2_entry.physical_address * ARCH_PAGE_SIZE);
    auto &pml1_entry = pml1->entries[pml1_index(virtual_address)];

    return pml1_entry.present;
}

uintptr_t virtual_to_physical(PML4 *pml4, uintptr_t virtual_address)
{
    ASSERT_INTERRUPTS_RETAINED();

    auto &pml4_entry = pml4->entries[pml4_index(virtual_address)];

    if (!pml4_entry.present)
    {
        return 0;
    }

    auto pml3 = reinterpret_cast<PML3 *>(pml4_entry.physical_address * ARCH_PAGE_SIZE);
    auto &pml3_entry = pml3->entries[pml3_index(virtual_address)];

    if (!pml3_entry.present)
    {
        return 0;
    }

    auto pml2 = reinterpret_cast<PML2 *>(pml3_entry.physical_address * ARCH_PAGE_SIZE);
    auto &pml2_entry = pml2->entries[pml2_index(virtual_address)];

    if (!pml2_entry.present)
    {
        return 0;
    }

    auto pml1 = reinterpret_cast<PML1 *>(pml2_entry.physical_address * ARCH_PAGE_SIZE);
    auto &pml1_entry = pml1->entries[pml1_index(virtual_address)];

    if (!pml1_entry.present)
    {
        return 0;
    }

    return (pml1_entry.physical_address * ARCH_PAGE_SIZE) + (virtual_address & 0xfff);
}

HjResult virtual_map(PML4 *pml4, MemoryRange physical_range, uintptr_t virtual_address, MemoryFlags flags)
{
    ASSERT_INTERRUPTS_RETAINED();

    for (size_t i = 0; i < physical_range.page_count(); i++)
    {
        uint64_t address = virtual_address + i * ARCH_PAGE_SIZE;

        auto pml4_entry = &pml4->entries[pml4_index(address)];
        auto pml3 = reinterpret_cast<PML3 *>(pml4_entry->physical_address * ARCH_PAGE_SIZE);

        if (!pml4_entry->present)
        {
            TRY(memory_alloc_identity(pml4, MEMORY_CLEAR, (uintptr_t *)&pml3));

            pml4_entry->present = 1;
            pml4_entry->writable = 1;
            pml4_entry->user = 1;
            pml4_entry->physical_address = (uint64_t)(pml3) / ARCH_PAGE_SIZE;
        }

        auto pml3_entry = &pml3->entries[pml3_index(address)];
        auto pml2 = reinterpret_cast<PML2 *>(pml3_entry->physical_address * ARCH_PAGE_SIZE);

        if (!pml3_entry->present)
        {
            TRY(memory_alloc_identity(pml4, MEMORY_CLEAR, (uintptr_t *)&pml2));

            pml3_entry->present = 1;
            pml3_entry->writable = 1;
            pml3_entry->user = 1;
            pml3_entry->physical_address = (uint64_t)(pml2) / ARCH_PAGE_SIZE;
        }

        auto pml2_entry = &pml2->entries[pml2_index(address)];
        auto pml1 = reinterpret_cast<PML1 *>(pml2_entry->physical_address * ARCH_PAGE_SIZE);

        if (!pml2_entry->present)
        {
            TRY(memory_alloc_identity(pml4, MEMORY_CLEAR, (uintptr_t *)&pml1));

            pml2_entry->present = 1;
            pml2_entry->writable = 1;
            pml2_entry->user = 1;
            pml2_entry->physical_address = (uint64_t)(pml1) / ARCH_PAGE_SIZE;
        }

        auto pml1_entry = &pml1->entries[pml1_index(address)];

        pml1_entry->present = 1;
        pml1_entry->writable = 1;
        pml1_entry->user = flags & MEMORY_USER;
        pml1_entry->physical_address = (physical_range.base() + i * ARCH_PAGE_SIZE) / ARCH_PAGE_SIZE;
    }

    paging_invalidate_tlb();

    return SUCCESS;
}

MemoryRange virtual_alloc(PML4 *pml4, MemoryRange physical_range, MemoryFlags flags)
{
    ASSERT_INTERRUPTS_RETAINED();

    bool is_user_memory = flags & MEMORY_USER;

    uintptr_t virtual_address = 0;
    size_t current_size = 0;

    // we skip the first page to make null deref trigger a page fault
    for (size_t i = (is_user_memory ? 256 : 1) * 1024; i < (is_user_memory ? 1024 : 256) * 1024; i++)
    {
        uintptr_t current_address = i * ARCH_PAGE_SIZE;

        if (!virtual_present(pml4, current_address))
        {
            if (current_size == 0)
            {
                virtual_address = current_address;
            }

            current_size += ARCH_PAGE_SIZE;

            if (current_size == physical_range.size())
            {
                assert(SUCCESS == virtual_map(pml4, physical_range, virtual_address, flags));

                return (MemoryRange){virtual_address, current_size};
            }
        }
        else
        {
            current_size = 0;
        }
    }

    system_panic("Out of virtual memory!");
}

void virtual_free(PML4 *pml4, MemoryRange virtual_range)
{
    ASSERT_INTERRUPTS_RETAINED();

    for (size_t i = 0; i < virtual_range.page_count(); i++)
    {
        uint64_t address = virtual_range.base() + i * ARCH_PAGE_SIZE;

        auto pml4_entry = &pml4->entries[pml4_index(address)];

        if (!pml4_entry->present)
        {
            continue;
        }

        auto pml3 = reinterpret_cast<PML3 *>(pml4_entry->physical_address * ARCH_PAGE_SIZE);
        auto pml3_entry = &pml3->entries[pml3_index(address)];

        if (!pml3_entry->present)
        {
            continue;
        }

        auto pml2 = reinterpret_cast<PML2 *>(pml3_entry->physical_address * ARCH_PAGE_SIZE);
        auto pml2_entry = &pml2->entries[pml2_index(address)];

        if (!pml2_entry->present)
        {
            continue;
        }

        auto pml1 = reinterpret_cast<PML1 *>(pml2_entry->physical_address * ARCH_PAGE_SIZE);
        auto pml1_entry = &pml1->entries[pml1_index(address)];

        *pml1_entry = {};
    }

    paging_invalidate_tlb();
}

PML4 *pml4_create()
{
    PML4 *pml4;
    memory_alloc_identity(kernel_pml4(), MEMORY_CLEAR, (uintptr_t *)&pml4);

    PML3 *pml3;
    memory_alloc_identity(kernel_pml4(), MEMORY_CLEAR, (uintptr_t *)&pml3);

    auto &pml4_entry = pml4->entries[0];
    pml4_entry.user = 1;
    pml4_entry.writable = 1;
    pml4_entry.present = 1;
    pml4_entry.physical_address = (uint64_t)pml3 / ARCH_PAGE_SIZE;

    PML2 *pml2;
    memory_alloc_identity(kernel_pml4(), MEMORY_CLEAR, (uintptr_t *)&pml2);

    auto &pml3_entry = pml3->entries[0];
    pml3_entry.user = 1;
    pml3_entry.writable = 1;
    pml3_entry.present = 1;
    pml3_entry.physical_address = (uint64_t)pml2 / ARCH_PAGE_SIZE;

    for (size_t i = 0; i < 512; i++)
    {
        auto &pml2_entry = pml2->entries[i];
        pml2_entry.user = 1;
        pml2_entry.writable = 1;
        pml2_entry.present = 1;
        pml2_entry.physical_address = (uint64_t)&kpml1[i] / ARCH_PAGE_SIZE;
    }

    return pml4;
}

void pml4_destroy(PML4 *pml4)
{
    return;
    ASSERT_INTERRUPTS_RETAINED();

    UNUSED(pml4);

    ASSERT_NOT_REACHED();
}

void pml4_switch(PML4 *pml4)
{
    paging_load_directory((uintptr_t)pml4);
}

} // namespace Arch::x86_64
