#include <assert.h>
#include <libsystem/Logger.h>

#include "kernel/handover/Handover.h"
#include "kernel/system/System.h"

static Handover _handover;

const char *entry_type_to_string[] = {
    "AVAILABLE",
    "RESERVED",
    "ACPI",
    "NVS",
    "BADRAM",
    "KERNEL",
};

Handover *handover()
{
    return &_handover;
}

void handover_assert(uint32_t magic)
{
    if (!(is_multiboot2(magic) ||
          is_stivale2(magic)))
    {
        logger_fatal("Wrong bootloader please use any multiboot/stival bootloader\n\tMagic number: 0x%08x!", magic);
    }
}

void handover_dump()
{
    logger_info("Bootloader: '%s'", _handover.bootloader);
    logger_info("Command lines: '%s'", _handover.command_line);

    logger_info("Memory map:");
    for (size_t i = 0; i < _handover.memory_map_size; i++)
    {
        MemoryMapEntry *entry = &_handover.memory_map[i];

        logger_info("\t%d: %08p-%08p: %s",
                    i,
                    entry->range.base(),
                    entry->range.base() + entry->range.size() - 1,
                    entry_type_to_string[entry->type]);
    }
    logger_info("\t -> Usable memory: %dKio", _handover.memory_usable / 1024);

    logger_info("Modules:");
    for (size_t i = 0; i < _handover.modules_size; i++)
    {
        Module *module = &_handover.modules[i];
        logger_info("\t%d: %08p-%08p: %s",
                    i,
                    module->range.base(),
                    module->range.base() + module->range.size() - 1,
                    module->command_line);
    }
    logger_info("\t-> %d module found", _handover.modules_size);
}

Handover *handover_initialize(void *header, uint32_t magic)
{
    handover_assert(magic);

    logger_info("Parsing handover informations...");
    logger_info("Header=%08x, Magic=%08x", header, magic);

    if (is_multiboot2(magic))
    {
        multiboot2_parse_header(&_handover, header);
    }
    else if (is_stivale2(magic))
    {
        stivale2_parse_header(&_handover, header);
    }
    else
    {
        ASSERT_NOT_REACHED();
    }

    handover_dump();

    return &_handover;
}
