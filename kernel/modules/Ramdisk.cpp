
#include <libfile/tar.h>
#include <libsystem/Logger.h>
#include <libsystem/Result.h>
#include <libsystem/core/CString.h>

#include "arch/VirtualMemory.h"

#include "kernel/filesystem/Filesystem.h"
#include "kernel/memory/Memory.h"
#include "kernel/modules/Modules.h"

void ramdisk_load(Module *module)
{
    TARBlock block;
    for (size_t i = 0; tar_read((void *)module->range.base(), &block, i); i++)
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
        else if ((block.typeflag & 8) == 0 || (block.typeflag & 8) == 5)
        {
            FsHandle *handle = nullptr;
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
                logger_error("Failed to write file: %s", result_to_string(result));
            }

            fshandle_destroy(handle);
        }
        else if (block.name[strlen(block.name) - 1] != '/')
        {
            Path *linkname = path_create(block.linkname);
            filesystem_mklink_for_tar(file_path, linkname);
            path_destroy(linkname);
        }

        path_destroy(file_path);
    }

    memory_free(arch_kernel_address_space(), module->range);

    logger_info("Loading ramdisk succeeded.");
}
