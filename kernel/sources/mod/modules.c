// This file is part of "skiftOS" licensed under the MIT License.
// See: LICENSE.md
// Project URL: github.com/maker-dev/skift

#include <string.h>
#include "kernel/modules.h"
#include "kernel/multiboot.h"
#include "kernel/logging.h"
#include "kernel/ramdisk.h"

void load_module(multiboot_module_t *module)
{
    info("Loading modules at 0x%x.", module);
    info("Module command line: '%s'.", module->cmdline);

    if (strcmp("ramdisk", (char*)module->cmdline) == 0)
    {
        ramdisk_load(module);
    }
    else
    {
        warn("Unknow module '%s'!", module->cmdline);
    }
}

uint modules_get_end(multiboot_info_t *minfo)
{
    multiboot_module_t * module = (multiboot_module_t*)minfo->mods_addr;

    for(size_t i = 0; i < minfo->mods_count; i++)
    {
        module = (multiboot_module_t *)((u32)module->mod_end + 1);
    }

    return (uint)module;
}

void modules_setup(multiboot_info_t *minfo)
{
    info("Bootloader: %s.", minfo->boot_loader_name);
    info("Kernel modules count: %d.", minfo->mods_count);
    info("Kernel modules adresse: 0x%x.", minfo->mods_addr);

    
    multiboot_module_t * module = (multiboot_module_t*)minfo->mods_addr;

    for(size_t i = 0; i < minfo->mods_count; i++)
    {
        load_module(module);
        module = (multiboot_module_t *)((u32)module->mod_end + 1);
    }

    info("%d modules loaded!", minfo->mods_count);
}