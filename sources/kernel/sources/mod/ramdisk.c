// This file is part of "skiftOS" licensed under the MIT License.
// See: LICENSE.md
// Project URL: github.com/maker-dev/skift

#include <string.h>

#include "kernel/filesystem.h"
#include "kernel/multiboot.h"
#include "kernel/logging.h"
#include "utils/tar.h"

void ramdisk_load(multiboot_module_t *module)
{
    info("Loading ramdisk at 0x%x...", module->mod_start);
    tar_block_t block;

    for(size_t i = 0; tar_read((void*)module->mod_start, &block, i); i++)
    {
        if (block.name[strlen(block.name) - 1] == '/')
        {
            // debug("Found folder: %s at 0x%x.", block.name, block.data);
            directory_create(NULL, block.name, 0);
        }
        else
        {
            // debug("Found file: %s at 0x%x.", block.name, block.data);
            file_create(NULL, block.name, 0);
        }
    }

    // filesystem_dump(NULL, "");
}