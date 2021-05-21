
/* modules.c : kernel modules/ramdisk loader                                  */

#include "system/Streams.h"
#include <string.h>

#include "system/modules/Modules.h"

void module_load(Module *module)
{
    if (strcmp("ramdisk", module->command_line) == 0)
    {
        ramdisk_load(module);
    }
    else
    {
        Kernel::logln("Unknow module '{}'!", module->command_line);
    }
}

void modules_initialize(Handover *handover)
{
    Kernel::logln("Loading modules:");
    for (size_t i = 0; i < handover->modules_size; i++)
    {
        Module *module = &handover->modules[i];

        Kernel::logln("\tModule {}: {p}-{p}: {}",
                      i,
                      module->range.base(),
                      module->range.base() + module->range.size() - 1,
                      module->command_line);

        module_load(module);
    }

    Kernel::logln("{} module loaded!", handover->modules_size);
}
