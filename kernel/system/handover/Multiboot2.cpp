#include "system/Streams.h"
#include <assert.h>
#include <string.h>

#include <thirdparty/multiboot/Multiboot2.h>

#include "system/handover/Handover.h"

bool is_multiboot2(uint32_t magic)
{
    return magic == MULTIBOOT2_BOOTLOADER_MAGIC;
}

const char *_multiboot2_tag_name[] = {
    "END",
    "CMDLINE",
    "BOOT_LOADER_NAME",
    "MODULE",
    "BASIC_MEMINFO",
    "BOOTDEV",
    "MMAP",
    "VBE",
    "FRAMEBUFFER",
    "ELF_SECTIONS",
    "APM",
    "EFI32",
    "EFI64",
    "SMBIOS",
    "ACPI_OLD",
    "ACPI_NEW",
    "NETWORK",
    "EFI_MMAP",
    "EFI_BS",
    "EFI32_IH",
    "EFI64_IH",
    "LOAD_BASE_ADDR",
};

void multiboot2_parse_memory_map(Handover *handover, struct multiboot_tag_mmap *tag)
{
    for (multiboot_memory_map_t *mmap = tag->entries;
         (uint8_t *)mmap < (uint8_t *)tag + tag->size;
         mmap = (multiboot_memory_map_t *)((uintptr_t)mmap + tag->entry_size))
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
}

void multiboot2_parse_module(Handover *handover, struct multiboot_tag_module *m)
{
    assert(handover->modules_size < HANDOVER_MODULES_SIZE);

    Module *module = &handover->modules[handover->modules_size];
    module->range = MemoryRange::around_non_aligned_address(m->mod_start, m->mod_end - m->mod_start);
    strncpy(module->command_line, (const char *)m->cmdline, HANDOVER_COMMAND_LINE_SIZE);
    handover->modules_size++;
}

void multiboot2_parse_framebuffer(Handover *handover, struct multiboot_tag_framebuffer_common *tag)
{
    handover->framebuffer_addr = tag->framebuffer_addr;
    handover->framebuffer_width = tag->framebuffer_width;
    handover->framebuffer_height = tag->framebuffer_height;
    handover->framebuffer_pitch = tag->framebuffer_pitch;
    handover->framebuffer_bpp = tag->framebuffer_bpp / 8;
}

void multiboot2_parse_header(Handover *handover, void *header_ptr)
{
    strcpy(handover->bootloader, "unknown");
    strcpy(handover->command_line, "");

    header_ptr = (void *)((uintptr_t)header_ptr + 8);
    struct multiboot_tag *tag = (struct multiboot_tag *)header_ptr;

    while (tag->type != MULTIBOOT_TAG_TYPE_END)
    {
        Kernel::logln("\t{08}: {}", tag, _multiboot2_tag_name[tag->type]);

        switch (tag->type)
        {
        case MULTIBOOT_TAG_TYPE_MMAP:
            multiboot2_parse_memory_map(handover, (struct multiboot_tag_mmap *)tag);
            break;

        case MULTIBOOT_TAG_TYPE_MODULE:
            multiboot2_parse_module(handover, (struct multiboot_tag_module *)tag);
            break;

        case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
            strncpy(handover->bootloader, ((struct multiboot_tag_string *)tag)->string, HANDOVER_BOOTLOADER_NAME_SIZE);
            break;

        case MULTIBOOT_TAG_TYPE_CMDLINE:
            strncpy(handover->command_line, ((struct multiboot_tag_string *)tag)->string, HANDOVER_COMMAND_LINE_SIZE);
            break;

        case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
            multiboot2_parse_framebuffer(handover, (struct multiboot_tag_framebuffer_common *)tag);
            break;

        case MULTIBOOT_TAG_TYPE_ACPI_OLD:
            handover->acpi_rsdp_address = (uintptr_t) & ((struct multiboot_tag_old_acpi *)tag)->rsdp;
            break;

        default:
            Kernel::logln("\t\t-> IGNORED");
            break;
        }

        tag = (struct multiboot_tag *)ALIGN_UP(
            (uintptr_t)tag + tag->size,
            MULTIBOOT_TAG_ALIGN);
    }
}
