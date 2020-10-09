#include <libsystem/Assert.h>
#include <libsystem/Logger.h>
#include <libsystem/core/CString.h>

#include <thirdparty/multiboot/Multiboot.h>

#include "kernel/handover/Handover.h"

bool is_multiboot1(uint32_t magic)
{
    return magic == MULTIBOOT_BOOTLOADER_MAGIC;
}

void multiboot1_parse_header(Handover *handover, void *header_ptr)
{
    multiboot_info_t *info = (multiboot_info_t *)header_ptr;

    strncpy(handover->bootloader, reinterpret_cast<char *>(info->boot_loader_name), HANDOVER_BOOTLOADER_NAME_SIZE);
    strncpy(handover->command_line, reinterpret_cast<char *>(info->cmdline), HANDOVER_COMMAND_LINE_SIZE);

    multiboot_module_t *m = reinterpret_cast<multiboot_module_t *>(info->mods_addr);
    for (size_t i = 0; i < info->mods_count; i++)
    {
        Module *module = &handover->modules[handover->modules_size];

        module->range = MemoryRange::around_non_aligned_address(m->mod_start, m->mod_end - m->mod_start);
        strncpy(module->command_line, reinterpret_cast<char *>(m->cmdline), HANDOVER_COMMAND_LINE_SIZE);

        m = reinterpret_cast<multiboot_module_t *>(m->mod_end + 1);
        handover->modules_size++;
    }

    for (multiboot_memory_map_t *mmap = reinterpret_cast<multiboot_memory_map_t *>(info->mmap_addr); (uintptr_t)mmap < info->mmap_addr + info->mmap_length; mmap = (multiboot_memory_map_t *)((uintptr_t)mmap + mmap->size + sizeof(mmap->size)))
    {
        assert(handover->memory_map_size < HANDOVER_MEMORY_MAP_SIZE);

        if ((mmap->addr > UINT32_MAX) ||
            (mmap->addr + mmap->len > UINT32_MAX))
        {
            continue;
        }

        if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE)
        {
            handover->memory_usable += mmap->len;
        }

        MemoryMapEntry *entry = &handover->memory_map[handover->memory_map_size];

        entry->range = MemoryRange::from_non_aligned_address(mmap->addr, mmap->len);

        switch (mmap->type)
        {
        case MULTIBOOT_MEMORY_AVAILABLE:
            entry->type = MEMORY_MAP_ENTRY_AVAILABLE;
            break;
        case MULTIBOOT_MEMORY_ACPI_RECLAIMABLE:
            entry->type = MEMORY_MAP_ENTRY_ACPI_RECLAIMABLE;
            break;
        case MULTIBOOT_MEMORY_NVS:
            entry->type = MEMORY_MAP_ENTRY_NVS;
            break;
        case MULTIBOOT_MEMORY_BADRAM:
            entry->type = MEMORY_MAP_ENTRY_BADRAM;
            break;
        default:
            entry->type = MEMORY_MAP_ENTRY_RESERVED;
            break;
        }

        handover->memory_map_size++;
    }

    handover->framebuffer_addr = info->framebuffer_addr;
    handover->framebuffer_width = info->framebuffer_width;
    handover->framebuffer_height = info->framebuffer_height;
    handover->framebuffer_pitch = info->framebuffer_pitch;
}
