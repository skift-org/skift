/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* modules.c : kernel modules/ramdisk loader                                  */

#include <libsystem/cstring.h>
#include <libsystem/logger.h>

#include <thirdparty/multiboot/Multiboot.h>

#include "kernel/modules.h"

void load_module(multiboot_module_t *module)
{
    if (strcmp("ramdisk", (char *)module->cmdline) == 0)
    {
        ramdisk_load(module);
    }
    else
    {
        logger_warn("Unknow module '%s'!", module->cmdline);
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