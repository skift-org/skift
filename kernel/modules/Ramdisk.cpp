
#include <libfile/TARArchive.h>
#include <libsystem/Logger.h>
#include <libsystem/Result.h>
#include <libsystem/core/CString.h>

#include "archs/VirtualMemory.h"

#include "kernel/filesystem/Filesystem.h"
#include "kernel/memory/Memory.h"
#include "kernel/modules/Modules.h"

void ramdisk_load(Module *module)
{
    TARBlock block;
    for (size_t i = 0; tar_read((void *)module->range.base(), &block, i); i++)
    {
        auto file_path = Path::parse(block.name);

        if (block.name[strlen(block.name) - 1] == '/')
        {
            Result result = filesystem_mkdir(file_path);

            if (result != SUCCESS)
            {
                logger_warn("Failed to create directory %s: %s", block.name, result_to_string(result));
            }
        }
        else if ((block.typeflag & 8) == 0 || (block.typeflag & 8) == 5)
        {
            auto result_or_handle = filesystem_open(file_path, OPEN_WRITE | OPEN_CREATE);

            if (!result_or_handle.success())
            {
                logger_warn("Failed to open file %s! %s", block.name, result_to_string(result_or_handle.result()));
                continue;
            }

            auto handle = result_or_handle.take_value();

            auto result_or_written = handle->write(block.data, block.size);

            if (!result_or_written.success())
            {
                logger_error("Failed to write file: %s", result_to_string(result_or_written.result()));
            }

            delete handle;
        }
    }

    memory_free(arch_kernel_address_space(), module->range);

    logger_info("Loading ramdisk succeeded.");
}
