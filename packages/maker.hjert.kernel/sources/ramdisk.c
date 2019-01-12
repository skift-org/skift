/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <math.h>
#include <string.h>
#include <skift/tar.h>
#include <skift/logger.h>

#include "kernel/filesystem.h"
#include "kernel/multiboot.h"

void *ramdisk;

void ramdisk_load(multiboot_module_t *module)
{
    sk_log(LOG_INFO, "Loading ramdisk at 0x%x...", module->mod_start);

    ramdisk = (void *)module->mod_start;

    tar_block_t block;
    for (size_t i = 0; tar_read(ramdisk, &block, i); i++)
    {
        if (block.name[strlen(block.name) - 1] == '/')
        {
            sk_log(LOG_DEBUG, "Creating %s directory...", block.name);
            filesystem_mkdir(block.name);
        }
        else
        {
            sk_log(LOG_DEBUG, "Loading file %s...", block.name);
            fsnode_t *file = filesystem_open(block.name, OPENOPT_WRITE | OPENOPT_CREATE);
            filesystem_write(file, 0, block.data, block.size);
            filesystem_close(file);
        }
    }

    sk_log(LOG_FINE, "Loading ramdisk succeeded.");
}
