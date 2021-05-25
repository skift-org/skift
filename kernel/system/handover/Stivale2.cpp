#include <string.h>
#include <thirdparty/stivale/stivale2.h>

#include "system/Streams.h"
#include "system/handover/Handover.h"

bool is_stivale2(uint32_t magic)
{
    return magic == 0x73747632;
}

void stivale2_parse_header(Handover *handover, void *header_ptr)
{
    auto info = reinterpret_cast<stivale2_struct *>(header_ptr);
    strlcpy(handover->bootloader, info->bootloader_brand, HANDOVER_BOOTLOADER_NAME_SIZE);

    auto tag = reinterpret_cast<stivale2_tag *>(info->tags);

    while (tag)
    {
        switch (tag->identifier)
        {

        case STIVALE2_STRUCT_TAG_CMDLINE_ID:
        {
            auto command_line = reinterpret_cast<stivale2_struct_tag_cmdline *>(tag);
            strlcpy(handover->command_line, (const char *)command_line->cmdline, HANDOVER_COMMAND_LINE_SIZE);
        }
        break;

        case STIVALE2_STRUCT_TAG_MEMMAP_ID:
        {
            auto memory_map = reinterpret_cast<stivale2_struct_tag_memmap *>(tag);

            for (size_t i = 0; i < memory_map->entries; i++)
            {
                auto entry = &memory_map->memmap[i];

                if ((entry->base > UINTPTR_MAX) ||
                    (entry->base + entry->length > UINTPTR_MAX))
                {
                    continue;
                }

                if (entry->type == STIVALE2_MMAP_USABLE ||
                    entry->type == STIVALE2_MMAP_KERNEL_AND_MODULES)
                {
                    handover->memory_usable += entry->length;
                }

                MemoryMapEntry *e = &handover->memory_map[handover->memory_map_size];
                e->range = MemoryRange::from_non_aligned_address(entry->base, entry->length);

                switch (entry->type)
                {
                case STIVALE2_MMAP_USABLE:
                case STIVALE2_MMAP_KERNEL_AND_MODULES:
                    e->type = MEMORY_MAP_ENTRY_AVAILABLE;
                    break;
                case STIVALE2_MMAP_ACPI_RECLAIMABLE:
                    e->type = MEMORY_MAP_ENTRY_ACPI_RECLAIMABLE;
                    break;
                case STIVALE2_MMAP_ACPI_NVS:
                    e->type = MEMORY_MAP_ENTRY_NVS;
                    break;
                case STIVALE2_MMAP_BAD_MEMORY:
                    e->type = MEMORY_MAP_ENTRY_BADRAM;
                    break;
                default:
                    e->type = MEMORY_MAP_ENTRY_RESERVED;
                    break;
                }

                handover->memory_map_size++;
            }

            break;
        }
        case STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID:
        {
            auto fb = reinterpret_cast<stivale2_struct_tag_framebuffer *>(tag);

            handover->framebuffer_addr = fb->framebuffer_addr;
            handover->framebuffer_width = fb->framebuffer_width;
            handover->framebuffer_height = fb->framebuffer_height;
            handover->framebuffer_pitch = fb->framebuffer_pitch;
            handover->framebuffer_bpp = fb->framebuffer_bpp;

            break;
        }

        case STIVALE2_STRUCT_TAG_MODULES_ID:
        {
            auto modules = reinterpret_cast<stivale2_struct_tag_modules *>(tag);

            for (size_t i = 0; i < modules->module_count; i++)
            {
                auto module = &modules->modules[i];

                auto m = &handover->modules[handover->modules_size];
                m->range = MemoryRange::around_non_aligned_address(module->begin, module->end - module->begin);
                strncpy(m->command_line, (const char *)module->string, HANDOVER_COMMAND_LINE_SIZE);
                handover->modules_size++;
            }
            break;
        }
        case STIVALE2_STRUCT_TAG_RSDP_ID:
        {
            auto acpi = reinterpret_cast<stivale2_struct_tag_rsdp *>(tag);
            handover->acpi_rsdp_address = (uintptr_t)(acpi->rsdp);
            break;
        }

        default:
            break;
        }

        tag = reinterpret_cast<stivale2_tag *>(tag->next);
    }
}
