
#include <libfile/tar.h>
#include <libmath/math.h>
#include <libsystem/CString.h>
#include <libsystem/Logger.h>
#include <libsystem/Result.h>

#include "kernel/filesystem/Filesystem.h"
#include "kernel/memory/Memory.h"
#include "kernel/modules/Modules.h"

void ramdisk_load(Module *module)
{
    tar_block_t block;
    for (size_t i = 0; tar_read((void *)module->range.base, &block, i); i++)
    {
        Path *file_path = path_create(block.name);

        if (block.name[strlen(block.name) - 1] == '/')
        {
            Result result = filesystem_mkdir(file_path);

            if (result < 0)
            {
                logger_warn("Failed to create directory %s: %s", block.name, result_to_string(result));
            }
        }
        else
        {
            FsHandle *handle = NULL;
            Result result = filesystem_open(file_path, OPEN_WRITE | OPEN_CREATE, &handle);

            if (result != SUCCESS)
            {
                logger_warn("Failed to open file %s! %s", block.name, result_to_string(result));
                continue;
            }

            size_t written = 0;
            result = fshandle_write(handle, block.data, block.size, &written);

            if (result != SUCCESS)
            {
                logger_error("Failled to write file: %s", result_to_string(result));
            }

            fshandle_destroy(handle);
        }

        path_destroy(file_path);
    }

    memory_free(memory_kpdir(), module->range.base, module->range.size / PAGE_SIZE, false);

    logger_info("Loading ramdisk succeeded.");
}
