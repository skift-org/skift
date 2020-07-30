#include <libsystem/Assert.h>
#include <libsystem/Logger.h>

#include "kernel/multiboot/Multiboot.h"
#include "kernel/system/System.h"

static Multiboot _multiboot;

const char *entry_type_to_string[] = {
    "AVAILABLE",
    "RESERVED",
    "ACPI",
    "NVS",
    "BADRAM",
};

void multiboot_assert(uint32_t magic)
{
    if (!(is_multiboot1(magic) || is_multiboot2(magic)))
    {
        system_panic("Wrong bootloader please use a GRUB or any multiboot2 bootloader\n\tMagic number: 0x%08x!", magic);
    }
}

int multiboot_version(uint32_t magic)
{
    if (is_multiboot1(magic))
    {
        return 1;
    }

    if (is_multiboot2(magic))
    {
        return 2;
    }

    ASSERT_NOT_REACHED();
}

void multiboot_dump()
{
    logger_info("Bootloader: '%s'", _multiboot.bootloader);
    logger_info("Command lines: '%s'", _multiboot.command_line);

    logger_info("Memory map:");
    for (size_t i = 0; i < _multiboot.memory_map_size; i++)
    {
        MemoryMapEntry *entry = &_multiboot.memory_map[i];

        logger_info("\t%d: %08p-%08p: %s",
                    i,
                    entry->range.base,
                    entry->range.base + entry->range.size - 1,
                    entry_type_to_string[entry->type]);
    }
    logger_info("\t -> Usable memory: %dKio", _multiboot.memory_usable / 1024);

    logger_info("Modules:");
    for (size_t i = 0; i < _multiboot.modules_size; i++)
    {
        Module *module = &_multiboot.modules[i];
        logger_info("\t%d: %08p-%08p: %s",
                    i,
                    module->range.base,
                    module->range.base + module->range.size - 1,
                    module->command_line);
    }
    logger_info("\t-> %d module found", _multiboot.modules_size);
}

Multiboot *multiboot_initialize(void *header, uint32_t magic)
{
    multiboot_assert(magic);
    logger_info("Booting from a multiboot-%d bootloader...", multiboot_version(magic));

    if (is_multiboot1(magic))
    {
        multiboot1_parse_header(&_multiboot, header);
    }
    else if (is_multiboot2(magic))
    {
        multiboot2_parse_header(&_multiboot, header);
    }
    else
    {
        ASSERT_NOT_REACHED();
    }

    multiboot_dump();

    return &_multiboot;
}
