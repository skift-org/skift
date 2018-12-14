/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <string.h>
#include <skift/logger.h>

#include "kernel/multiboot.h"

#include "kernel/modules.h"

void load_module(multiboot_module_t *module)
{
    if (strcmp("ramdisk", (char *)module->cmdline) == 0)
    {
        ramdisk_load(module);
    }
    else
    {
        sk_log(LOG_WARNING, "Unknow module '%s'!", module->cmdline);
    }
}

uint modules_get_end(multiboot_info_t *minfo)
{
    multiboot_module_t *module = (multiboot_module_t *)minfo->mods_addr;

    for (size_t i = 0; i < minfo->mods_count; i++)
    {
        module = (multiboot_module_t *)((u32)module->mod_end + 1);
    }

    return (uint)module;
}

void modules_setup(multiboot_info_t *minfo)
{
    multiboot_module_t *module = (multiboot_module_t *)minfo->mods_addr;

    for (size_t i = 0; i < minfo->mods_count; i++)
    {
        load_module(module);
        module = (multiboot_module_t *)((u32)module->mod_end + 1);
    }
}