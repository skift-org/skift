#include <libsystem/Logger.h>
#include <libsystem/core/CString.h>

#include "kernel/handover/Handover.h"
#include "kernel/handover/Stivale1.h"

bool is_stival1(uint32_t magic)
{
    return magic == 0x7374766c;
}

void stival1_parse_header(Handover *handover, void *header_ptr)
{
    auto info = reinterpret_cast<Stivale1Info *>(header_ptr);

    strncpy(handover->bootloader, "Stivale1 compliant", HANDOVER_BOOTLOADER_NAME_SIZE);
    strncpy(handover->command_line, reinterpret_cast<const char *>(info->cmdline), HANDOVER_COMMAND_LINE_SIZE);

    Stivale1Module *module = reinterpret_cast<Stivale1Module *>(info->modules);
    while (module)
    {
        Module *m = &handover->modules[handover->modules_size];
        m->range = MemoryRange::around_non_aligned_address(module->begin, module->end - module->begin);
        strncpy(m->command_line, (const char *)module->string, HANDOVER_COMMAND_LINE_SIZE);

        module = reinterpret_cast<Stivale1Module *>(module->next);
        handover->modules_size++;
    }

    Stivale1Memory *memory = reinterpret_cast<Stivale1Memory *>(info->memory_map_addr);

    for (size_t i = 0; i < info->memory_map_entries; i++)
    {
        Stivale1Memory *entry = &memory[i];

        if ((entry->base > UINT32_MAX) ||
            (entry->base + entry->length > UINT32_MAX))
        {
            continue;
        }

        if (entry->type == Stivale1MemoryType::USABLE ||
            entry->type == Stivale1MemoryType::KERNEL)
        {
            handover->memory_usable += entry->length;
        }

        MemoryMapEntry *e = &handover->memory_map[handover->memory_map_size];
        e->range = MemoryRange::from_non_aligned_address(entry->base, entry->length);

        switch (entry->type)
        {
        case Stivale1MemoryType::USABLE:
        case Stivale1MemoryType::KERNEL:
            e->type = MEMORY_MAP_ENTRY_AVAILABLE;
            break;
        case Stivale1MemoryType::ACPI_RECLAIMABLE:
            e->type = MEMORY_MAP_ENTRY_ACPI_RECLAIMABLE;
            break;
        case Stivale1MemoryType::NVS:
            e->type = MEMORY_MAP_ENTRY_NVS;
            break;
        case Stivale1MemoryType::BADRAM:
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
