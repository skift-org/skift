// This file is part of "skiftOS" licensed under the MIT License.
// See: LICENSE.md
// Project URL: github.com/maker-dev/skift

#include <string.h>
#include <math.h>

#include "kernel/filesystem.h"
#include "kernel/multiboot.h"
#include "kernel/logging.h"
#include "utils/tar.h"

int rd_file_open(file_t *file);
void rd_file_close(file_t *file);
int rd_file_read(file_t *file, uint offset, void *buffer, uint n);
int rd_file_write(file_t *file, uint offset, void *buffer, uint n);
void rd_file_stat(file_t *file, fstat_t *stat);


filesystem_t ramdisk_fs;
void * ramdisk;

void ramdisk_load(multiboot_module_t *module)
{
    info("Loading ramdisk at 0x%x...", module->mod_start);

    ramdisk = (void *)module->mod_start;
    ramdisk_fs.file_open = rd_file_open;
    ramdisk_fs.file_close = rd_file_close;
    ramdisk_fs.file_read = rd_file_read;
    ramdisk_fs.file_stat = rd_file_stat;

    tar_block_t block;
    for (size_t i = 0; tar_read(ramdisk, &block, i); i++)
    {
        if (block.name[strlen(block.name) - 1] == '/')
        {
            // debug("Found folder: %s at 0x%x.", block.name, block.data);
            directory_create(NULL, block.name, 0);
        }
        else
        {
            // debug("Found file: %s at 0x%x.", block.name, block.data);
            file_create(NULL, block.name, &ramdisk_fs, 0, i);
        }
    }

    // filesystem_dump(NULL, "");
}

int rd_file_open(file_t *file)
{
    debug("File opened %s", file->name);
    tar_block_t block;
    return tar_read(ramdisk, &block, file->inode);
}

void rd_file_close(file_t *file)
{
    debug("File closed %s", file->name);
    UNUSED(file);
}

int rd_file_read(file_t *file, uint offset, void *buffer, uint n)
{
    tar_block_t block;
    tar_read(ramdisk, &block, file->inode);

    n = min(block.size - min(offset, block.size), n);

    memcpy(buffer, block.data + offset, n);

    debug("File readed %s", file->name);
    return n;
}

int rd_file_write(file_t *file, uint offset, void *buffer, uint n)
{
    UNUSED(file + offset + (int)buffer + n);
    return 0;
}

void rd_file_stat(file_t *file, fstat_t *stat)
{
    tar_block_t block;
    tar_read(ramdisk, &block, file->inode);

    stat->size = block.size;
    stat->read = 1;
    stat->write = 0;
}
