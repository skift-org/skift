#include <libsystem/assert.h>
#include <libsystem/cstring.h>
#include <libsystem/logger.h>

#include <thirdparty/multiboot/Multiboot2.h>

#include "kernel/multiboot/Multiboot.h"

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

void multiboot2_parse_memory_map(Multiboot *multiboot, struct multiboot_tag_mmap *tag)
{
    for (multiboot_memory_map_t *mmap = tag->entries;
         (uint8_t *)mmap < (uint8_t *)tag + tag->size;
         mmap = (multiboot_memory_map_t *)((uintptr_t)mmap + tag->entry_size))
    {
        assert(multiboot->memory_map_size < MULTIBOOT_MEMORY_MAP_SIZE);

        if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE)
        {
            multiboot->memory_usable += mmap->len;
        }

        MemoryMapEntry *entry = &multiboot->memory_map[multiboot->memory_map_size];

        entry->range = memory_range_from_non_aligned_address(mmap->addr, mmap->len);

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

        multiboot->memory_map_size++;
    }
}

void multiboot2_parse_module(Multiboot *multiboot, struct multiboot_tag_module *m)
{
    assert(multiboot->modules_size < MULTIBOOT_MODULES_SIZE);

    Module *module = &multiboot->modules[multiboot->modules_size];
    module->range = memory_range_around_non_aligned_address(m->mod_start, m->mod_end - m->mod_start);
    strncpy(module->command_line, (const char *)m->cmdline, MULTIBOOT_COMMAND_LINE_SIZE);
    multiboot->modules_size++;
}

void multiboot2_parse_header(Multiboot *multiboot, void *header_ptr)
{
    __unused(multiboot);
    __unused(header_ptr);

    header_ptr = (void *)((uintptr_t)header_ptr + 8);

    struct multiboot_tag *tag = (struct multiboot_tag *)header_ptr;

    strcpy(multiboot->bootloader, "unknown");
    strcpy(multiboot->command_line, "");

    while (tag->type != MULTIBOOT_TAG_TYPE_END)
    {
        logger_info("\t%08x: %s", tag, _multiboot2_tag_name[tag->type]);

        switch (tag->type)
        {
        case MULTIBOOT_TAG_TYPE_MMAP:
            multiboot2_parse_memory_map(multiboot, (struct multiboot_tag_mmap *)tag);
            break;

        case MULTIBOOT_TAG_TYPE_MODULE:
            multiboot2_parse_module(multiboot, (struct multiboot_tag_module *)tag);
            break;

        case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
            strncpy(multiboot->bootloader, ((struct multiboot_tag_string *)tag)->string, MULTIBOOT_BOOTLOADER_NAME_SIZE);
            break;

        case MULTIBOOT_TAG_TYPE_CMDLINE:
            strncpy(multiboot->command_line, ((struct multiboot_tag_string *)tag)->string, MULTIBOOT_COMMAND_LINE_SIZE);
            break;

        default:
            logger_warn("\t\t-> IGNORED");
            break;
        }

        tag = (struct multiboot_tag *)__align_up(
            (uintptr_t)tag + tag->size,
            MULTIBOOT_TAG_ALIGN);
    }
}