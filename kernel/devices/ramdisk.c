/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libmath/math.h>
#include <libsystem/cstring.h>
#include <libsystem/logger.h>
#include <libfile/tar.h>

#include "filesystem/filesystem.h"
#include "multiboot.h"

void ramdisk_load(multiboot_module_t *module)
{
    // Extract the ramdisk tar archive.

    logger_info("Loading ramdisk at 0x%x...", module->mod_start);

    void *ramdisk = (void *)module->mod_start;

    tar_block_t block;
    for (size_t i = 0; tar_read(ramdisk, &block, i); i++)
    {
        Path *file_path = path(block.name);

        if (block.name[strlen(block.name) - 1] == '/')
        {
            if (filesystem_mkdir(ROOT, file_path) != 0)
            {
                logger_warn("Failed to create directory %s...", block.name);
            }
        }
        else
        {
            stream_t *s = filesystem_open(ROOT, file_path, IOSTREAM_WRITE | IOSTREAM_CREATE | IOSTREAM_TRUNC);

            if (s != NULL)
            {
                filesystem_write(s, block.data, block.size);
                filesystem_close(s);
            }
            else
            {
                logger_warn("Failed to open file %s!", block.name);
            }
        }

        path_delete(file_path);
    }

    logger_info("Loading ramdisk succeeded.");
}
