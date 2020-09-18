#include <libsystem/Logger.h>
#include <libsystem/core/CString.h>

#include <thirdparty/limine/stivale/stivale.h>

#include "kernel/handover/Handover.h"

bool is_stivale1(uint32_t magic)
{
    return magic == 0x7374766c;
}

void stivale1_parse_header(Handover *handover, void *header_ptr)
{
    auto info = reinterpret_cast<stivale_struct *>(header_ptr);

    strncpy(handover->bootloader, "Stivale1 compliant", HANDOVER_BOOTLOADER_NAME_SIZE);
    strncpy(handover->command_line, reinterpret_cast<const char *>(info->cmdline), HANDOVER_COMMAND_LINE_SIZE);

    auto module = reinterpret_cast<stivale_module *>(info->modules);
    while (module)
    {
        Module *m = &handover->modules[handover->modules_size];
        m->range = MemoryRange::around_non_aligned_address(module->begin, module->end - module->begin);
        strncpy(m->command_line, (const char *)module->string, HANDOVER_COMMAND_LINE_SIZE);

        module = reinterpret_cast<stivale_module *>(module->next);
        handover->modules_size++;
    }

    auto memory = reinterpret_cast<stivale_mmap_entry *>(info->memory_map_addr);

    for (size_t i = 0; i < info->memory_map_entries; i++)
    {
        auto entry = &memory[i];

        if ((entry->base > UINT32_MAX) ||
            (entry->base + entry->length > UINT32_MAX))
        {
            continue;
        }

        if (entry->type == STIVALE_MMAP_USABLE ||
            entry->type == STIVALE_MMAP_KERNEL_AND_MODULES)
        {
            handover->memory_usable += entry->length;
        }

        MemoryMapEntry *e = &handover->memory_map[handover->memory_map_size];
        e->range = MemoryRange::from_non_aligned_address(entry->base, entry->length);

        switch (entry->type)
        {
        case STIVALE_MMAP_USABLE:
        case STIVALE_MMAP_KERNEL_AND_MODULES:
            e->type = MEMORY_MAP_ENTRY_AVAILABLE;
            break;
        case STIVALE_MMAP_ACPI_RECLAIMABLE:
            e->type = MEMORY_MAP_ENTRY_ACPI_RECLAIMABLE;
            break;
        case STIVALE_MMAP_ACPI_NVS:
            e->type = MEMORY_MAP_ENTRY_NVS;
            break;
        case STIVALE_MMAP_BAD_MEMORY:
            e->type = MEMORY_MAP_ENTRY_BADRAM;
            break;
        default:
            e->type = MEMORY_MAP_ENTRY_RESERVED;
            break;
        }

        handover->memory_map_size++;
    }

    handover->acpi_rsdp_address = info->rsdp;

    handover->framebuffer_addr = info->framebuffer_addr;
    handover->framebuffer_width = info->framebuffer_width;
    handover->framebuffer_height = info->framebuffer_height;
    handover->framebuffer_pitch = info->framebuffer_pitch;
}
