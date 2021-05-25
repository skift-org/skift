#include "system/Streams.h"
#include <assert.h>

#include "system/handover/Handover.h"
#include "system/system/System.h"

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
        system_panic("Wrong bootloader please use any multiboot/stival bootloader\n\tMagic number: 0x%08x!", magic);
    }
}

void handover_dump()
{
    Kernel::logln("Bootloader: '{}'", _handover.bootloader);
    Kernel::logln("Command lines: '{}'", _handover.command_line);

    Kernel::logln("Memory map:");
    for (size_t i = 0; i < _handover.memory_map_size; i++)
    {
        MemoryMapEntry *entry = &_handover.memory_map[i];

        Kernel::logln("\t{}: {p}-{p}: {}",
                      i,
                      entry->range.base(),
                      entry->range.base() + entry->range.size() - 1,
                      entry_type_to_string[entry->type]);
    }
    Kernel::logln("\t -> Usable memory: {}Kio", _handover.memory_usable / 1024);

    Kernel::logln("Modules:");
    for (size_t i = 0; i < _handover.modules_size; i++)
    {
        Module *module = &_handover.modules[i];
        Kernel::logln("\t{}: {p}-{p}: {}",
                      i,
                      module->range.base(),
                      module->range.base() + module->range.size() - 1,
                      module->command_line);
    }
    Kernel::logln("\t-> {} module found", _handover.modules_size);
}

Handover *handover_initialize(void *header, uint32_t magic)
{
    handover_assert(magic);

    Kernel::logln("Parsing handover informations...");
    Kernel::logln("Header={08x}, Magic={08x}", header, magic);

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
