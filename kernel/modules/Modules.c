
/* modules.c : kernel modules/ramdisk loader                                  */

#include <libsystem/core/CString.h>
#include <libsystem/system/Logger.h>

#include "kernel/modules/Modules.h"

void module_load(Module *module)
{
    if (strcmp("ramdisk", module->command_line) == 0)
    {
        ramdisk_load(module);
    }
    else
    {
        logger_warn("Unknow module '%s'!", module->command_line);
    }
}

void modules_initialize(Multiboot *multiboot)
{
    logger_info("Loading modules:");
    for (size_t i = 0; i < multiboot->modules_size; i++)
    {
        Module *module = &multiboot->modules[i];

        logger_info("\tModule %d: %08x-%08x: %s",
                    i,
                    module->range.base,
                    module->range.base + module->range.size - 1,
                    module->command_line);

        module_load(module);
    }

    logger_info("%d module loaded!", multiboot->modules_size);
}
